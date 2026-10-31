# -------------------------------------------------------------------------
# Flash this block directly into Raspberry Pi Pico running MicroPython
# -------------------------------------------------------------------------
import rp2
from machine import Pin

# PIO program executing 40 kHz square wave with configurable initial phase delay
# Clock divisor = 125.0 -> 1 PIO cycle = 1.0 microsecond (1 us per tick)
# 40 kHz period = 25 cycles (12 cycles HIGH, 13 cycles LOW)
@rp2.asm_pio(set_init=(rp2.PIO.OUT_LOW, rp2.PIO.OUT_HIGH))
def pio_40khz_phased_emitter():
    # 1. Pull phase delay ticks from TX FIFO (in microseconds, 0 to 24)
    pull(block)
    mov(x, osr)
    
    # 2. Execute initial phase offset loop
    label("delay_loop")
    jmp(x_dec, "delay_loop")
    
    # 3. Continuous 40 kHz 50% duty cycle carrier loop (25 us period)
    wrap_target()
    set(pins, 0b01) [11]    # Pin A HIGH, Pin B LOW (1 + 11 = 12 us)
    set(pins, 0b10) [12]    # Pin A LOW, Pin B HIGH (1 + 12 = 13 us)
    wrap()

# Initialize 4 State Machines across GPIO0..GPIO7
pins = [Pin(i, Pin.OUT) for i in range(8)]
sms = []
for sm_id in range(4):
    pin_base = sm_id * 2
    sm = rp2.StateMachine(sm_id, pio_40khz_phased_emitter, freq=1_000_000, set_base=pins[pin_base])
    sms.append(sm)

# Load progressive phase offsets: 0us, 3us, 6us, 9us (45 deg increments)
phase_delays_us = [0, 3, 6, 9]
for sm, delay in zip(sms, phase_delays_us):
    sm.put(delay)
    sm.active(1)

print("[OK] RP2040 PIO ultrasonic phased engine running on GPIO0..GPIO7 @ 40.000 kHz")