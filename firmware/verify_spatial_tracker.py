import math
import numpy as np
from PIL import Image, ImageDraw

GRID_DIM = 8
FOV_DEG = 63.0
HALF_FOV_RAD = math.radians(FOV_DEG / 2.0)
TAN_HALF_FOV = math.tan(HALF_FOV_RAD)
SENSOR_MOUNT_Z = 0.023

KALMAN_Q_POS = 0.00004
KALMAN_Q_VEL = 0.00200
KALMAN_R_BASE = 0.00008

SPRING_K0 = 180.0
SPRING_GAMMA = 4.2
DAMPING_B = 1.45
WALL_Z = 0.055
TRIGGER_ENTER_Z = WALL_Z + 0.020


class Kalman6D:
    def __init__(self, init_pos=(0.0, 0.0, 0.055)):
        self.pos = np.array(init_pos, dtype=np.float64)
        self.vel = np.zeros(3, dtype=np.float64)
        self.P = np.zeros((6, 6), dtype=np.float64)
        for i in range(3):
            self.P[i, i] = 0.010
            self.P[i+3, i+3] = 0.500
        self.active_ticks = 0
        self.is_locked = False

    def predict(self, dt):
        self.pos += self.vel * dt
        for i in range(3):
            self.P[i, i] += (self.P[i+3, i+3] * dt * dt) + KALMAN_Q_POS
            self.P[i, i+3] += (self.P[i+3, i+3] * dt)
            self.P[i+3, i] += (self.P[i+3, i+3] * dt)
            self.P[i+3, i+3] += KALMAN_Q_VEL

    def update(self, measured, dt, noise_scale=1.0):
        self.predict(dt)
        res = measured - self.pos
        r_var = KALMAN_R_BASE * noise_scale
        for i in range(3):
            S = self.P[i, i] + r_var
            K_pos = self.P[i, i] / S
            K_vel = self.P[i+3, i] / S

            self.pos[i] += K_pos * res[i]
            self.vel[i] += K_vel * res[i]

            self.P[i, i] *= (1.0 - K_pos)
            self.P[i, i+3] *= (1.0 - K_pos)
            self.P[i+3, i] -= (K_vel * self.P[i, i])
            self.P[i+3, i+3] -= (K_vel * self.P[i, i+3])

        self.active_ticks += 1
        if self.active_ticks > 3:
            self.is_locked = True


def generate_simulated_tof_frame(finger_pos_m, hand_pos_m, noise_std_m=0.0018):
    distances_mm = np.full((GRID_DIM, GRID_DIM), 350, dtype=np.int32)
    statuses = np.full((GRID_DIM, GRID_DIM), 255, dtype=np.uint8)

    for r in range(GRID_DIM):
        for c in range(GRID_DIM):
            norm_x = (c - 3.5) / 3.5
            norm_y = (3.5 - r) / 3.5

            ray_dir = np.array([norm_x * TAN_HALF_FOV, norm_y * TAN_HALF_FOV, 1.0])
            ray_dir /= np.linalg.norm(ray_dir)

            t_finger = finger_pos_m[2] / ray_dir[2]
            pt_on_finger = ray_dir * t_finger
            dist_to_finger = np.linalg.norm(pt_on_finger - finger_pos_m)

            t_hand = hand_pos_m[2] / ray_dir[2]
            pt_on_hand = ray_dir * t_hand
            dist_to_hand = np.linalg.norm(pt_on_hand - hand_pos_m)

            if dist_to_finger <= 0.015:
                # Direct line-of-sight ray intercept
                meas_dist = np.linalg.norm(pt_on_finger) + np.random.normal(0, noise_std_m)
                distances_mm[r, c] = int(meas_dist * 1000.0)
                statuses[r, c] = 5
            elif dist_to_hand <= 0.035:
                meas_dist = np.linalg.norm(pt_on_hand) + np.random.normal(0, noise_std_m * 1.5)
                distances_mm[r, c] = int(meas_dist * 1000.0)
                statuses[r, c] = 5

    return distances_mm, statuses


def run_spatial_tracker_verification():
    print("====================================================================")
    print("  IRON CAVE INDUSTRIES - TIME-OF-FLIGHT SPATIAL TRACKER VERIFIER")
    print("  Sensor: VL53L5CX 8x8 Multizone | Rate: 60 Hz | Closed-Loop Forcefield")
    print("====================================================================")

    dt = 1.0 / 60.0
    total_frames = 180

    t_eval = np.linspace(0, total_frames * dt, total_frames)

    finger_traj = []
    hand_traj = []
    for t in t_eval:
        fx = 0.015 * math.cos(2.0 * math.pi * 0.8 * t)
        fy = 0.012 * math.sin(2.0 * math.pi * 0.8 * t)
        if t < 0.6:
            fz = 0.110 - (0.110 - 0.075) * (t / 0.6)
        elif t < 1.8:
            fz = 0.075 - 0.032 * math.sin(math.pi * (t - 0.6) / 1.2)
        else:
            fz = 0.075 + (0.120 - 0.075) * ((t - 1.8) / 1.2)

        hx = fx + 0.010
        hy = fy - 0.025
        hz = fz + 0.045
        finger_traj.append(np.array([fx, fy, fz]))
        hand_traj.append(np.array([hx, hy, hz]))

    kf_finger = Kalman6D(init_pos=finger_traj[0])
    kf_hand = Kalman6D(init_pos=hand_traj[0])

    est_finger_pos = []
    est_finger_vel = []
    forcefield_active = []
    radiation_pressures = []
    normal_forces = []
    stiffness_values = []
    penetrations = []
    acoustic_targets = []

    last_raw_depth_map = None

    for i in range(total_frames):
        true_f = finger_traj[i]
        true_h = hand_traj[i]

        dists, stats = generate_simulated_tof_frame(true_f, true_h)
        if i == 75:
            last_raw_depth_map = dists.copy()

        pts = []
        for r in range(GRID_DIM):
            for c in range(GRID_DIM):
                if stats[r, c] == 5:
                    d_m = dists[r, c] * 0.001
                    norm_x = (c - 3.5) / 3.5
                    norm_y = (3.5 - r) / 3.5
                    ray = np.array([norm_x * TAN_HALF_FOV, norm_y * TAN_HALF_FOV, 1.0])
                    ray /= np.linalg.norm(ray)
                    pt_3d = ray * d_m
                    pts.append((pt_3d[0], pt_3d[1], pt_3d[2]))

        if pts:
            pts.sort(key=lambda p: p[2])
            meas_finger = np.array(pts[0])
            meas_hand = np.mean(np.array(pts), axis=0)

            kf_finger.update(meas_finger, dt, noise_scale=1.0)
            kf_hand.update(meas_hand, dt, noise_scale=1.5)
        else:
            kf_finger.predict(dt)
            kf_hand.predict(dt)

        est_f = kf_finger.pos.copy()
        est_v = kf_finger.vel.copy()
        est_finger_pos.append(est_f)
        est_finger_vel.append(est_v)

        ff_on = False
        p_rad = 0.0
        f_norm = 0.0
        k_val = 0.0
        pen_m = 0.0
        tgt = np.array([0.0, 0.0, 0.055])

        if kf_finger.is_locked and est_f[2] <= TRIGGER_ENTER_Z:
            ff_on = True
            raw_pen = WALL_Z - est_f[2]
            if raw_pen < 0.0:
                pen_m = 0.0
                k_val = SPRING_K0 * 0.15
                p_rad = 25.0
                f_norm = 1.2
            else:
                pen_m = raw_pen
                norm_pen = min(1.0, raw_pen / 0.035)
                k_val = SPRING_K0 * (1.0 + SPRING_GAMMA * norm_pen * norm_pen)
                f_spring = k_val * raw_pen
                f_damp = max(0.0, -DAMPING_B * est_v[2])
                f_tot = f_spring + f_damp
                f_norm = f_tot * 1000.0
                p_rad = min(280.0, f_tot / 0.000100)

            tgt = np.array([
                np.clip(est_f[0] + est_v[0] * (dt * 1.5), -0.060, 0.060),
                np.clip(est_f[1] + est_v[1] * (dt * 1.5), -0.060, 0.060),
                WALL_Z - (pen_m * 0.5)
            ])

        forcefield_active.append(ff_on)
        radiation_pressures.append(p_rad)
        normal_forces.append(f_norm)
        stiffness_values.append(k_val)
        penetrations.append(pen_m)
        acoustic_targets.append(tgt)

    est_finger_pos = np.array(est_finger_pos)
    true_finger_pos = np.array(finger_traj)
    pos_errors_mm = np.linalg.norm(est_finger_pos - true_finger_pos, axis=1) * 1000.0
    mean_err_mm = np.mean(pos_errors_mm[10:])
    max_err_mm = np.max(pos_errors_mm[10:])

    max_p_rad = max(radiation_pressures)
    max_force_mn = max(normal_forces)
    num_triggers = sum(forcefield_active)

    print(f"[TRACKER-VERIFY] Processed Frames         : {total_frames} @ 60 Hz")
    print(f"[TRACKER-VERIFY] Mean 3D Tracking Error   : {mean_err_mm:.2f} mm (63 deg FOV coarse 8x8 discretization)")
    print(f"[TRACKER-VERIFY] Max 3D Tracking Error    : {max_err_mm:.2f} mm")
    print(f"[TRACKER-VERIFY] Forcefield Active Ticks  : {num_triggers} / {total_frames} ({num_triggers*100.0/total_frames:.1f}%)")
    print(f"[TRACKER-VERIFY] Peak Radiation Pressure  : {max_p_rad:.1f} Pa (Target >= 150 Pa)")
    print(f"[TRACKER-VERIFY] Peak Normal Restoring F  : {max_force_mn:.1f} mN")
    print("====================================================================")

    img_w, img_h = 1920, 1080
    im = Image.new("RGB", (img_w, img_h), (18, 20, 24))
    draw = ImageDraw.Draw(im)

    def draw_box(x, y, w, h, title):
        draw.rectangle([x, y, x + w, y + h], fill=(26, 29, 35), outline=(50, 56, 68), width=2)
        draw.rectangle([x, y, x + w, y + 34], fill=(34, 38, 46))
        draw.text((x + 16, y + 9), title, fill=(235, 89, 20))

    draw_box(40, 40, 580, 480, "PANEL 1: 3D TRAJECTORY & KALMAN FINGERTIP ESTIMATE")
    draw_box(660, 40, 580, 480, "PANEL 2: 8x8 VL53L5CX MULTIZONE DEPTH MAP (FRAME 75)")
    draw_box(1280, 40, 600, 480, "PANEL 3: POSITION TRACKING RESIDUALS & 60Hz ERROR")
    draw_box(40, 560, 580, 480, "PANEL 4: FORCEFIELD TRIGGER & PENETRATION DYNAMICS")
    draw_box(660, 560, 580, 480, "PANEL 5: ACOUSTIC RADIATION PRESSURE & NORMAL FORCE")
    draw_box(1280, 560, 600, 480, "PANEL 6: CLOSED-LOOP ACOUSTIC FOCAL INTERCEPTION")

    # Panel 1: XY Trajectory
    p1_ox, p1_oy = 40 + 290, 40 + 260
    draw.line([p1_ox - 220, p1_oy, p1_ox + 220, p1_oy], fill=(60, 65, 78), width=1)
    draw.line([p1_ox, p1_oy - 200, p1_ox, p1_oy + 200], fill=(60, 65, 78), width=1)
    scale_xy = 7500.0
    for k in range(len(true_finger_pos) - 1):
        x1 = p1_ox + int(true_finger_pos[k, 0] * scale_xy)
        y1 = p1_oy - int(true_finger_pos[k, 1] * scale_xy)
        x2 = p1_ox + int(true_finger_pos[k+1, 0] * scale_xy)
        y2 = p1_oy - int(true_finger_pos[k+1, 1] * scale_xy)
        draw.line([x1, y1, x2, y2], fill=(120, 130, 150), width=2)

    for k in range(len(est_finger_pos) - 1):
        x1 = p1_ox + int(est_finger_pos[k, 0] * scale_xy)
        y1 = p1_oy - int(est_finger_pos[k, 1] * scale_xy)
        x2 = p1_ox + int(est_finger_pos[k+1, 0] * scale_xy)
        y2 = p1_oy - int(est_finger_pos[k+1, 1] * scale_xy)
        c = (0, 229, 255) if forcefield_active[k] else (0, 255, 128)
        draw.line([x1, y1, x2, y2], fill=c, width=3)

    # Panel 2: Depth map heatmap
    p2_x, p2_y = 660 + 65, 40 + 65
    cell_size = 45
    for r in range(GRID_DIM):
        for c in range(GRID_DIM):
            d_val = last_raw_depth_map[r, c] if last_raw_depth_map is not None else 200
            norm = np.clip((d_val - 40) / 140.0, 0.0, 1.0)
            red = int(255 * (1.0 - norm))
            blue = int(255 * norm)
            green = int(180 * math.sin(norm * math.pi))
            cx = p2_x + c * cell_size
            cy = p2_y + r * cell_size
            draw.rectangle([cx, cy, cx + cell_size - 2, cy + cell_size - 2], fill=(red, green, blue))
            draw.text((cx + 8, cy + 14), f"{d_val}", fill=(255, 255, 255))

    # Panel 3: Error plot
    p3_x, p3_y = 1280 + 50, 40 + 100
    p3_w, p3_h = 500, 320
    draw.line([p3_x, p3_y + p3_h, p3_x + p3_w, p3_y + p3_h], fill=(80, 88, 105), width=2)
    draw.line([p3_x, p3_y, p3_x, p3_y + p3_h], fill=(80, 88, 105), width=2)
    draw.text((p3_x + 10, p3_y + 10), f"Mean 3D Error: {mean_err_mm:.2f} mm", fill=(0, 255, 180))
    draw.text((p3_x + 10, p3_y + 35), f"Max Residual : {max_err_mm:.2f} mm", fill=(255, 200, 80))
    for k in range(len(pos_errors_mm) - 1):
        x1 = p3_x + int(k * (p3_w / total_frames))
        y1 = p3_y + p3_h - int((pos_errors_mm[k] / 20.0) * p3_h)
        x2 = p3_x + int((k+1) * (p3_w / total_frames))
        y2 = p3_y + p3_h - int((pos_errors_mm[k+1] / 20.0) * p3_h)
        draw.line([x1, y1, x2, y2], fill=(0, 229, 255), width=2)

    # Panel 4: Forcefield Penetration
    p4_x, p4_y = 40 + 50, 560 + 100
    p4_w, p4_h = 480, 320
    draw.line([p4_x, p4_y + p4_h, p4_x + p4_w, p4_y + p4_h], fill=(80, 88, 105), width=2)
    draw.line([p4_x, p4_y, p4_x, p4_y + p4_h], fill=(80, 88, 105), width=2)
    for k in range(total_frames - 1):
        x1 = p4_x + int(k * (p4_w / total_frames))
        y1 = p4_y + p4_h - int(penetrations[k] * 7000.0)
        x2 = p4_x + int((k+1) * (p4_w / total_frames))
        y2 = p4_y + p4_h - int(penetrations[k+1] * 7000.0)
        draw.line([x1, y1, x2, y2], fill=(235, 89, 20), width=3)
    draw.text((p4_x + 10, p4_y + 10), "Virtual Wall Z = 55mm", fill=(200, 200, 200))
    draw.text((p4_x + 10, p4_y + 35), f"Max Penetration: {max(penetrations)*1000.0:.1f} mm", fill=(235, 89, 20))

    # Panel 5: Pressure & Force
    p5_x, p5_y = 660 + 50, 560 + 100
    p5_w, p5_h = 480, 320
    draw.line([p5_x, p5_y + p5_h, p5_x + p5_w, p5_y + p5_h], fill=(80, 88, 105), width=2)
    draw.line([p5_x, p5_y, p5_x, p5_y + p5_h], fill=(80, 88, 105), width=2)
    for k in range(total_frames - 1):
        x1 = p5_x + int(k * (p5_w / total_frames))
        y1 = p5_y + p5_h - int((radiation_pressures[k] / 300.0) * p5_h)
        x2 = p5_x + int((k+1) * (p5_w / total_frames))
        y2 = p5_y + p5_h - int((radiation_pressures[k+1] / 300.0) * p5_h)
        draw.line([x1, y1, x2, y2], fill=(255, 60, 60), width=3)
    draw.text((p5_x + 10, p5_y + 10), f"Peak Acoustic Pressure: {max_p_rad:.1f} Pa", fill=(255, 60, 60))
    draw.text((p5_x + 10, p5_y + 35), f"Peak Tactile Force     : {max_force_mn:.1f} mN", fill=(255, 180, 60))

    # Panel 6: Acoustic intercept target
    p6_x, p6_y = 1280 + 50, 560 + 100
    p6_w, p6_h = 500, 320
    draw.line([p6_x, p6_y + p6_h, p6_x + p6_w, p6_y + p6_h], fill=(80, 88, 105), width=2)
    draw.line([p6_x, p6_y, p6_x, p6_y + p6_h], fill=(80, 88, 105), width=2)
    for k in range(total_frames - 1):
        x1 = p6_x + int(k * (p6_w / total_frames))
        y1 = p6_y + p6_h - int((acoustic_targets[k][2] / 0.120) * p6_h)
        x2 = p6_x + int((k+1) * (p6_w / total_frames))
        y2 = p6_y + p6_h - int((acoustic_targets[k+1][2] / 0.120) * p6_h)
        draw.line([x1, y1, x2, y2], fill=(0, 229, 255), width=2)
    draw.text((p6_x + 10, p6_y + 10), "Closed-Loop Solver Target Z (m)", fill=(0, 229, 255))
    draw.text((p6_x + 10, p6_y + 35), "Dynamic Lead Compensation: ACTIVE", fill=(0, 255, 128))

    out_path = "n:/Iron Cave Industries/HPVE_Project/firmware/spatial_tracker_verification.png"
    im.save(out_path)
    print(f"[TRACKER-VERIFY] Saved 6-Panel Verification Dashboard -> {out_path}")
    return max_p_rad >= 150.0 and num_triggers > 50


if __name__ == "__main__":
    success = run_spatial_tracker_verification()
    exit(0 if success else 1)
