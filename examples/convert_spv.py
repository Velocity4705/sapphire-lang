#!/usr/bin/env python3
import sys

def convert_spv_to_cpp(input_file, var_name):
    with open(input_file, 'rb') as f:
        data = f.read()
    
    # SPIR-V is uint32_t aligned
    uint32_data = []
    for i in range(0, len(data), 4):
        if i + 4 <= len(data):
            # Little-endian conversion
            value = (data[i] | 
                    (data[i+1] << 8) | 
                    (data[i+2] << 16) | 
                    (data[i+3] << 24))
            uint32_data.append(value)
    
    # Print as C++ array
    print(f"const uint32_t {var_name}[] = {{")
    for i in range(0, len(uint32_data), 8):
        chunk = uint32_data[i:i+8]
        hex_values = [f"0x{v:08x}" for v in chunk]
        print("    " + ",".join(hex_values) + ",")
    print("};")
    print(f"// Size: {len(uint32_data)} uint32_t values ({len(data)} bytes)")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: convert_spv.py <input.spv> <var_name>")
        sys.exit(1)
    
    convert_spv_to_cpp(sys.argv[1], sys.argv[2])
