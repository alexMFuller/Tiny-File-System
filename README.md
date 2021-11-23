# Tiny-File-System
Creates a virtual disk in memory and provides the user a shell to interact with it

Known bugs with Export missing chars at the end of the virtual file and adding garbage characters with larger files.







Implemented Commands:

import LP tp

copy a file stored in the regular file system to the current TFS-disk. You might consider using GetFileSize to check to see if there is enough space on the TFS-disk for the new file.

export tp LP

copy a file stored in the current virtual disk to the regular file system.

ls tp

list the contents of the given directory of the TFS-disk

display

shown the raw contents of the current TFS-disk, as a table of 16 rows and columns where each entry is a 2 character hexadecimal number.

open LP

open an existing TFS-disk file, and closes the TFS-file currently in use, if any. Fails if the file does not exist.

create LP

create a new, empty TFS-disk file. (You must initialize the free space list and root directory) Fails if the file already exists.

exit

end the program after any pending updates to the TFS-disk file have been made.

mkdir tp

Create a new directory in the in the TFS-disk.


