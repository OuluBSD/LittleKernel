#!/usr/bin/env python3
"""
Configuration Header Generator
This script reads the .config file and generates a C++ header with #defines
"""

import os
import re

def generate_config_header(config_file=".config", header_file="kernel_config_defines.h"):
    """Generate C++ header file from .config file."""
    
    if not os.path.exists(config_file):
        print(f"Error: {config_file} does not exist")
        return False
    
    defines = []
    
    with open(config_file, 'r') as f:
        for line in f:
            line = line.strip()
            
            # Skip empty lines and comments
            if not line or line.startswith('#'):
                # Handle disabled config options (e.g., "# CONFIG_USB is not set")
                if line.startswith('# CONFIG_') and 'is not set' in line:
                    config_name = line[2:line.find(' is not set')]
                    defines.append((config_name, '0', 'disabled'))
                continue
            
            # Parse lines like "CONFIG_NAME=value"
            if '=' in line:
                parts = line.split('=', 1)
                config_name = parts[0].strip()
                config_value = parts[1].strip()
                
                # Handle quoted values
                if config_value.startswith('"') and config_value.endswith('"'):
                    config_value = config_value.strip('"')
                    defines.append((config_name, f'"{config_value}"', 'string'))
                elif config_value.lower() in ['y', 'yes', 'true']:
                    defines.append((config_name, '1', 'boolean'))
                elif config_value.lower() in ['n', 'no', 'false']:
                    defines.append((config_name, '0', 'boolean'))
                else:
                    # Assume numeric value
                    defines.append((config_name, config_value, 'numeric'))
    
    # Write header file
    with open(header_file, 'w') as f:
        f.write("/* Auto-generated from .config - DO NOT EDIT */\n")
        f.write("#ifndef _KERNEL_CONFIG_DEFINES_H\n")
        f.write("#define _KERNEL_CONFIG_DEFINES_H\n")
        f.write("\n")
        
        for name, value, type_desc in defines:
            f.write(f"#ifndef {name}\n")  # Only define if not already defined
            f.write(f"#define {name} {value}  /* {type_desc} */\n")
            f.write("#endif\n")
            f.write("\n")
        
        f.write("#endif /* _KERNEL_CONFIG_DEFINES_H */\n")
    
    print(f"Generated {header_file} with {len(defines)} configuration defines")
    return True

if __name__ == "__main__":
    generate_config_header()