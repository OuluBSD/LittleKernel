#!/usr/bin/env python3

import sys
import os

def update_floppy_image(floppy_image_path, kernel_path):
    """
    Updates a floppy image by adding or replacing the kernel file using mtools
    """
    print(f"Updating {floppy_image_path} with kernel from {kernel_path}")
    
    # Check if files exist
    if not os.path.exists(floppy_image_path):
        print(f"Error: Floppy image {floppy_image_path} does not exist")
        sys.exit(1)
        
    if not os.path.exists(kernel_path):
        print(f"Error: Kernel file {kernel_path} does not exist")
        sys.exit(1)
    
    # Create a temporary mtools configuration file
    import tempfile
    with tempfile.NamedTemporaryFile(mode='w', delete=False, suffix='.mtoolsrc') as mtoolsrc:
        mtoolsrc.write(f"drive z: file=\"{floppy_image_path}\"\n")
        mtoolsrc_path = mtoolsrc.name
    
    try:
        # Import subprocess to run mtools commands
        import subprocess
        
        # Make sure the target directory exists in the image (root directory is already there)
        # Try to create the kernel file using mcopy
        result = subprocess.run([
            'mcopy', 
            '-i', floppy_image_path,  # Use -i option directly
            '-D', 'o',  # Overwrite if exists
            kernel_path,
            'z:',
        ], env={**os.environ, 'MTOOLSRC': mtoolsrc_path}, capture_output=True, text=True)
        
        if result.returncode != 0:
            print(f"Error running mcopy: {result.stderr}")
            # The error might be because the filesystem is not standard FAT
            # Let's try creating the file directly using mmd first, then copy
            result2 = subprocess.run([
                'mmd', 
                '-i', floppy_image_path,
                'z:/KERNEL'
            ], env={**os.environ, 'MTOOLSRC': mtoolsrc_path}, capture_output=True, text=True)
            # Actually, mmd is for directories, not files
            
            # Try mcopy again
            result = subprocess.run([
                'mcopy', 
                '-i', floppy_image_path,
                '-D', 'o',  # Overwrite if exists
                kernel_path,
                'z:/KERNEL'
            ], env={**os.environ, 'MTOOLSRC': mtoolsrc_path}, capture_output=True, text=True)
            
            if result.returncode != 0:
                print(f"Final mcopy error: {result.stderr}")
                sys.exit(1)
        
        print("Successfully updated kernel in floppy image!")
        
    except Exception as e:
        print(f"Error updating floppy image: {str(e)}")
        sys.exit(1)
    finally:
        # Clean up the temporary configuration file
        os.unlink(mtoolsrc_path)

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python3 update_fat_image.py <floppy_image_path> <kernel_path>")
        sys.exit(1)
    
    floppy_image_path = sys.argv[1]
    kernel_path = sys.argv[2]
    
    update_floppy_image(floppy_image_path, kernel_path)