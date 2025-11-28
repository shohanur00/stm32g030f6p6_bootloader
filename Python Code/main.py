import serial
import time

# -------- CONFIG -------------
HEX_FILE = "main_project.hex"
SERIAL_PORT = "COM4"
BAUD = 115200
# ------------------------------

ser = serial.Serial(SERIAL_PORT, BAUD, timeout=0.2)
time.sleep(1)

ext_addr = 0  # TYPE 04 upper address
last_echo = ""


# -------- PARSE Intel HEX line ----------
def process_hex_line(line):
    if not line.startswith(":"):
        return None
    byte_count  = int(line[1:3], 16)
    address     = int(line[3:7], 16)
    record_type = int(line[7:9], 16)
    data_hex    = line[9:9 + byte_count * 2]
    checksum    = int(line[9 + byte_count * 2:9 + byte_count * 2 + 2], 16)
    return byte_count, address, record_type, data_hex, checksum


# ---- WAIT UNTIL WHOLE <...> FRAME RETURNED ----
def read_frame(timeout=0.3):
    frame = ""
    start_found = False
    start_time = time.time()

    while True:
        if time.time() - start_time > timeout:
            return "<TIMEOUT>"

        ch = ser.read().decode(errors="ignore")
        if not ch:
            continue

        if ch == "<":
            start_found = True
            frame = "<"
            continue

        if start_found:
            frame += ch
            if ch == ">":
                return frame


# Compute checksum: sum of all bytes modulo 256, two's complement
def calc_frame_checksum(addr, data_hex):
    # Address as 4 bytes big endian
    addr_bytes = addr.to_bytes(4, 'big')
    
    # Convert hex string data to bytes
    data_bytes = bytes.fromhex(data_hex)
    
    total = sum(addr_bytes) + sum(data_bytes)
    
    checksum = (-total) & 0xFF   # Two's complement, 8-bit
    return checksum

# -------- SEND record + wait echo ---------
def send_record(addr, data_hex):
    global last_echo

    # Flush old/stale UART data
    ser.reset_input_buffer()

    checksum = calc_frame_checksum(addr, data_hex)

    frame = f"<{addr:08X}:{data_hex}:{checksum:02X}>"
    ser.write(frame.encode())

    # WAIT MCU echo full <...> FRAME
    echo = read_frame()

    # Print both sent and received
    print("SENT:", frame)
    print("MCU :", echo)
    
    last_echo = echo
# ---------------- MAIN LOOP ----------------
with open(HEX_FILE, "r") as f:
    for line in f:
        line = line.strip()
        parsed = process_hex_line(line)
        if parsed is None:
            continue

        byte_count, address, record_type, data_hex, checksum = parsed

        if record_type == 4:
            ext_addr = int(data_hex, 16)    # update upper 16 bits
            continue

        if record_type == 0:
            full_addr = (ext_addr << 16) | address
            send_record(full_addr, data_hex)
            # time.sleep(0.01)  # small delay between frames

        if record_type == 1:
            print("End Of File")
            break

ser.close()
