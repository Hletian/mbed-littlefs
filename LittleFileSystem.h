/* mbed Microcontroller Library
 * Copyright (c) 2006-2012 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MBED_LFSFILESYSTEM_H
#define MBED_LFSFILESYSTEM_H

#include "FileSystem.h"
#include "BlockDevice.h"
extern "C" {
#include "lfs.h"
}

using namespace mbed;


/**
 * LittleFileSystem, a little filesystem
 */
class LittleFileSystem : public FileSystem {
public:
    /** Lifetime of the LittleFileSystem
     *
     *  @param name         Name to add filesystem to tree as
     *  @param bd           BlockDevice to mount, may be passed instead to mount call
     *  @param read_size
     *      Minimum size of a block read. This determines the size of read buffers.
     *      This may be larger than the physical read size to improve performance
     *      by caching more of the block device.
     *  @param prog_size
     *      Minimum size of a block program. This determines the size of program
     *      buffers. This may be larger than the physical program size to improve
     *      performance by caching more of the block device.
     *  @param block_size
     *      Size of an erasable block. This does not impact ram consumption and
     *      may be larger than the physical erase size. However, this should be
     *      kept small as each file currently takes up an entire block.
     *  @param lookahead
     *      Number of blocks to lookahead during block allocation. A larger
     *      lookahead reduces the number of passes required to allocate a block.
     *      The lookahead buffer requires only 1 bit per block so it can be quite
     *      large with little ram impact. Should be a multiple of 32.
     */
    LittleFileSystem(const char *name=NULL, BlockDevice *bd=NULL,
            lfs_size_t read_size=MBED_LFS_READ_SIZE,
            lfs_size_t prog_size=MBED_LFS_PROG_SIZE,
            lfs_size_t block_size=MBED_LFS_BLOCK_SIZE,
            lfs_size_t lookahead=MBED_LFS_LOOKAHEAD);
    virtual ~LittleFileSystem();
    
    /** Formats a logical drive, FDISK partitioning rule.
     *
     *  The block device to format should be mounted when this function is called.
     *
     *  @param bd       This is the block device that will be formated.
     *  @param read_size
     *      Minimum size of a block read. This determines the size of read buffers.
     *      This may be larger than the physical read size to improve performance
     *      by caching more of the block device.
     *  @param prog_size
     *      Minimum size of a block program. This determines the size of program
     *      buffers. This may be larger than the physical program size to improve
     *      performance by caching more of the block device.
     *  @param block_size
     *      Size of an erasable block. This does not impact ram consumption and
     *      may be larger than the physical erase size. However, this should be
     *      kept small as each file currently takes up an entire block.
     *  @param lookahead
     *      Number of blocks to lookahead during block allocation. A larger
     *      lookahead reduces the number of passes required to allocate a block.
     *      The lookahead buffer requires only 1 bit per block so it can be quite
     *      large with little ram impact. Should be a multiple of 32.
     */
    static int format(BlockDevice *bd,
        lfs_size_t read_size=MBED_LFS_READ_SIZE,
        lfs_size_t prog_size=MBED_LFS_PROG_SIZE,
        lfs_size_t block_size=MBED_LFS_BLOCK_SIZE,
        lfs_size_t lookahead=MBED_LFS_LOOKAHEAD);

    /** Mounts a filesystem to a block device
     *
     *  @param bd       BlockDevice to mount to
     *  @return         0 on success, negative error code on failure
     */
    virtual int mount(BlockDevice *bd);

    /** Unmounts a filesystem from the underlying block device
     *
     *  @return         0 on success, negative error code on failure
     */
    virtual int unmount();

    /** Remove a file from the filesystem.
     *
     *  @param path     The name of the file to remove.
     *  @return         0 on success, negative error code on failure
     */
    virtual int remove(const char *path);

    /** Rename a file in the filesystem.
     *
     *  @param path     The name of the file to rename.
     *  @param newpath  The name to rename it to
     *  @return         0 on success, negative error code on failure
     */
    virtual int rename(const char *path, const char *newpath);

    /** Store information about the file in a stat structure
     *
     *  @param path     The name of the file to find information about
     *  @param st       The stat buffer to write to
     *  @return         0 on success, negative error code on failure
     */
    virtual int stat(const char *path, struct stat *st);

    /** Create a directory in the filesystem.
     *
     *  @param path     The name of the directory to create.
     *  @param mode     The permissions with which to create the directory
     *  @return         0 on success, negative error code on failure
     */
    virtual int mkdir(const char *path, mode_t mode);

protected:
    /** Open a file on the filesystem
     *
     *  @param file     Destination for the handle to a newly created file
     *  @param path     The name of the file to open
     *  @param flags    The flags to open the file in, one of O_RDONLY, O_WRONLY, O_RDWR,
     *                  bitwise or'd with one of O_CREAT, O_TRUNC, O_APPEND
     *  @return         0 on success, negative error code on failure
     */
    virtual int file_open(fs_file_t *file, const char *path, int flags);

    /** Close a file
     *
     *  @param file     File handle
     *  return          0 on success, negative error code on failure
     */
    virtual int file_close(fs_file_t file);

    /** Read the contents of a file into a buffer
     *
     *  @param file     File handle
     *  @param buffer   The buffer to read in to
     *  @param size     The number of bytes to read
     *  @return         The number of bytes read, 0 at end of file, negative error on failure
     */
    virtual ssize_t file_read(fs_file_t file, void *buffer, size_t len);

    /** Write the contents of a buffer to a file
     *
     *  @param file     File handle
     *  @param buffer   The buffer to write from
     *  @param size     The number of bytes to write 
     *  @return         The number of bytes written, negative error on failure
     */
    virtual ssize_t file_write(fs_file_t file, const void *buffer, size_t len);

    /** Flush any buffers associated with the file
     *
     *  @param file     File handle
     *  @return         0 on success, negative error code on failure
     */
    virtual int file_sync(fs_file_t file);

    /** Move the file position to a given offset from from a given location
     *
     *  @param file     File handle
     *  @param offset   The offset from whence to move to
     *  @param whence   The start of where to seek
     *      SEEK_SET to start from beginning of file,
     *      SEEK_CUR to start from current position in file,
     *      SEEK_END to start from end of file
     *  @return         The new offset of the file
     */
    virtual off_t file_seek(fs_file_t file, off_t offset, int whence);

    /** Get the file position of the file
     *
     *  @param file     File handle
     *  @return         The current offset in the file
     */
    virtual off_t file_tell(fs_file_t file);

    /** Get the size of the file
     *
     *  @param file     File handle
     *  @return         Size of the file in bytes
     */
    virtual off_t file_size(fs_file_t file);

    /** Open a directory on the filesystem
     *
     *  @param dir      Destination for the handle to the directory
     *  @param path     Name of the directory to open
     *  @return         0 on success, negative error code on failure
     */
    virtual int dir_open(fs_dir_t *dir, const char *path);

    /** Close a directory
     *
     *  @param dir      Dir handle
     *  return          0 on success, negative error code on failure
     */
    virtual int dir_close(fs_dir_t dir);

    /** Read the next directory entry
     *
     *  @param dir      Dir handle
     *  @param ent      The directory entry to fill out
     *  @return         1 on reading a filename, 0 at end of directory, negative error on failure
     */
    virtual ssize_t dir_read(fs_dir_t dir, struct dirent *ent);

    /** Set the current position of the directory
     *
     *  @param dir      Dir handle
     *  @param offset   Offset of the location to seek to,
     *                  must be a value returned from dir_tell
     */
    virtual void dir_seek(fs_dir_t dir, off_t offset);

    /** Get the current position of the directory
     *
     *  @param dir      Dir handle
     *  @return         Position of the directory that can be passed to dir_rewind
     */
    virtual off_t dir_tell(fs_dir_t dir);

    /** Rewind the current position to the beginning of the directory
     *
     *  @param dir      Dir handle
     */
    virtual void dir_rewind(fs_dir_t dir);
    
private:
    lfs_t _lfs; // _the actual filesystem
    struct lfs_config _config;
    BlockDevice *_bd; // the block device

    // default parameters
    const lfs_size_t _read_size;
    const lfs_size_t _prog_size;
    const lfs_size_t _block_size;
    const lfs_size_t _lookahead;
};


#endif