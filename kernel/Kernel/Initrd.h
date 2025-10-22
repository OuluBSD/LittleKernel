#ifndef _Kernel_Initrd_h_
#define _Kernel_Initrd_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declarations
struct FsNode;

// Structure of initrd header
struct InitrdHeader {
    uint32 nfiles; // Number of files in the ramdisk
};

// Structure of initrd file header
struct InitrdFileHeader {
    uint8 magic;     // Magic number to identify valid initrd
    int8 name[64];   // Filename
    uint32 offset;   // Offset from start of initrd
    uint32 length;   // File length
};

// Initialize the initial ramdisk
FsNode* InitialiseInitrd(uint32 initrd_location);

#endif