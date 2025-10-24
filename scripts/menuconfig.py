#!/usr/bin/env python3

import os
import sys
import tempfile
import subprocess

def load_config(config_file):
    """Load configuration from .config file."""
    config = {}
    if os.path.exists(config_file):
        with open(config_file, 'r') as f:
            for line in f:
                line = line.strip()
                if line and not line.startswith('#'):
                    if '=' in line:
                        key, value = line.split('=', 1)
                        # Handle quoted values
                        if value.startswith('"') and value.endswith('"'):
                            value = value[1:-1]
                        elif value == 'y':
                            value = True
                        elif value == 'n':
                            value = False
                        else:
                            try:
                                value = int(value)
                            except ValueError:
                                pass  # Keep as string
                        config[key] = value
                    elif line.startswith('#') and ' is not set' in line:
                        # Handle commented-out options
                        key = line[2:line.find(' is not set')]
                        config[key] = False
    return config

def save_config(config, config_file):
    """Save configuration to .config file."""
    with open(config_file, 'w') as f:
        f.write("# LittleKernel Configuration\n")
        f.write("\n")
        for key, value in sorted(config.items()):
            if isinstance(value, bool):
                if value:
                    f.write(f"{key}=y\n")
                else:
                    f.write(f"#{key} is not set\n")
            elif isinstance(value, int):
                f.write(f"{key}={value}\n")
            else:
                f.write(f"{key}=\"{value}\"\n")

def use_dialog_interface():
    """Use the dialog-based configuration interface."""
    script_path = os.path.join(os.path.dirname(__file__), "configure_kernel.sh")
    if os.path.exists(script_path):
        try:
            subprocess.run([script_path], check=True)
            return True
        except subprocess.CalledProcessError:
            print("Error running dialog configuration interface.")
            return False
    else:
        print(f"Dialog configuration script not found: {script_path}")
        return False

def display_menu(config):
    """Display the configuration menu."""
    while True:
        os.system('clear' if os.name == 'posix' else 'cls')
        print("LittleKernel Configuration")
        print("=========================")
        print()
        print("1. Use dialog-based configuration (recommended)")
        print("2. Use text-based configuration")
        print("3. View current configuration")
        print("4. Save and exit")
        print("5. Exit without saving")
        print()
        
        choice = input("Enter your choice (1-5): ").strip()
        
        if choice == '1':
            if use_dialog_interface():
                # Reload config after dialog interface
                config.update(load_config(".config"))
        elif choice == '2':
            display_text_menu(config)
        elif choice == '3':
            view_config(config)
        elif choice == '4':
            save_config(config, '.config')
            print("Configuration saved to .config")
            break
        elif choice == '5':
            break
        else:
            print("Invalid choice. Press Enter to continue...")
            input()

def display_text_menu(config):
    """Display the text-based configuration menu."""
    while True:
        os.system('clear' if os.name == 'posix' else 'cls')
        print("LittleKernel Configuration - Text Mode")
        print("=====================================")
        print()
        print("1. Toggle boolean options")
        print("2. Set integer options")
        print("3. Set string options")
        print("4. View current configuration")
        print("5. Save and exit")
        print("6. Return to main menu")
        print()
        
        choice = input("Enter your choice (1-6): ").strip()
        
        if choice == '1':
            toggle_bool_options(config)
        elif choice == '2':
            set_int_options(config)
        elif choice == '3':
            set_string_options(config)
        elif choice == '4':
            view_config(config)
        elif choice == '5':
            save_config(config, '.config')
            print("Configuration saved to .config")
            break
        elif choice == '6':
            return
        else:
            print("Invalid choice. Press Enter to continue...")
            input()

def toggle_bool_options(config):
    """Display and toggle boolean options."""
    bool_options = {k: v for k, v in config.items() if isinstance(v, bool)}
    
    os.system('clear' if os.name == 'posix' else 'cls')
    print("Boolean Options")
    print("===============")
    print()
    
    if not bool_options:
        print("No boolean options found.")
        input("Press Enter to continue...")
        return
    
    for i, (key, value) in enumerate(bool_options.items(), 1):
        state = 'Y' if value else 'N'
        print(f"{i:2d}. [{state}] {key}")
    
    print()
    print("0. Back to main menu")
    print()
    
    try:
        choice = int(input("Enter option number to toggle (0 to go back): ").strip())
        if choice == 0:
            return
        elif 1 <= choice <= len(bool_options):
            key = list(bool_options.keys())[choice - 1]
            config[key] = not config[key]
            print(f"Toggled {key} to {'y' if config[key] else 'n'}")
            input("Press Enter to continue...")
        else:
            print("Invalid option number.")
            input("Press Enter to continue...")
    except ValueError:
        print("Invalid input. Please enter a number.")
        input("Press Enter to continue...")

def set_int_options(config):
    """Display and set integer options."""
    int_options = {k: v for k, v in config.items() if isinstance(v, int)}
    
    os.system('clear' if os.name == 'posix' else 'cls')
    print("Integer Options")
    print("===============")
    print()
    
    if not int_options:
        print("No integer options found.")
        input("Press Enter to continue...")
        return
    
    for i, (key, value) in enumerate(int_options.items(), 1):
        print(f"{i:2d}. {key} = {value}")
    
    print()
    print("0. Back to main menu")
    print()
    
    try:
        choice = int(input("Enter option number to modify (0 to go back): ").strip())
        if choice == 0:
            return
        elif 1 <= choice <= len(int_options):
            key = list(int_options.keys())[choice - 1]
            current = config[key]
            new_val = input(f"Enter new value for {key} (current: {current}): ").strip()
            try:
                config[key] = int(new_val)
                print(f"Set {key} to {config[key]}")
            except ValueError:
                print(f"Invalid value. Keeping current value: {current}")
            input("Press Enter to continue...")
        else:
            print("Invalid option number.")
            input("Press Enter to continue...")
    except ValueError:
        print("Invalid input. Please enter a number.")
        input("Press Enter to continue...")

def set_string_options(config):
    """Display and set string options."""
    str_options = {k: v for k, v in config.items() if isinstance(v, str) and not isinstance(v, bool)}
    
    os.system('clear' if os.name == 'posix' else 'cls')
    print("String Options")
    print("==============")
    print()
    
    if not str_options:
        print("No string options found.")
        input("Press Enter to continue...")
        return
    
    for i, (key, value) in enumerate(str_options.items(), 1):
        print(f"{i:2d}. {key} = \"{value}\"")
    
    print()
    print("0. Back to main menu")
    print()
    
    try:
        choice = int(input("Enter option number to modify (0 to go back): ").strip())
        if choice == 0:
            return
        elif 1 <= choice <= len(str_options):
            key = list(str_options.keys())[choice - 1]
            current = config[key]
            new_val = input(f"Enter new value for {key} (current: \"{current}\"): ").strip()
            config[key] = new_val
            print(f"Set {key} to \"{config[key]}\"")
            input("Press Enter to continue...")
        else:
            print("Invalid option number.")
            input("Press Enter to continue...")
    except ValueError:
        print("Invalid input. Please enter a number.")
        input("Press Enter to continue...")

def view_config(config):
    """View the current configuration."""
    os.system('clear' if os.name == 'posix' else 'cls')
    print("Current Configuration")
    print("=====================")
    print()
    
    for key, value in sorted(config.items()):
        if isinstance(value, bool):
            if value:
                print(f"{key}=y")
            else:
                print(f"#{key} is not set")
        elif isinstance(value, int):
            print(f"{key}={value}")
        else:
            print(f"{key}=\"{value}\"")
    
    print()
    input("Press Enter to continue...")

def main():
    # Default config file path
    config_file = '.config'
    
    # Load existing config
    config = load_config(config_file)
    
    # Add some default options if config is empty
    if not config:
        config = {
            'CONFIG_X86': True,
            'CONFIG_SERIAL_CONSOLE': True,
            'CONFIG_VGA_CONSOLE': True,
            'CONFIG_KERNEL_DEBUG': True,
            'CONFIG_VERBOSE_LOG': False,
            'CONFIG_RUNTIME_CONFIG': True,
            'CONFIG_HAL': True,
            'CONFIG_PROFILING': False,
            'CONFIG_MODULES': True,
            'CONFIG_PCI': True,
            'CONFIG_TIMER_HZ': 100,
            'CONFIG_MAX_PROCESSES': 128,
            'CONFIG_KERNEL_HEAP_SIZE': 16,
            'CONFIG_EARLY_MEM': True,
            'CONFIG_HW_DIAGNOSTICS': True,
            'CONFIG_ERROR_HANDLING': True
        }
    
    # Display menu and handle user input
    display_menu(config)

if __name__ == "__main__":
    main()