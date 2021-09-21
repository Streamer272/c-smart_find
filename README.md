# Smart Find
<hr>

### What is it?
Smart Find is program used to find string in given directory, show results and help user manage their files

### How do I use it?
Simply, you can just type
`sf --text="text you want to find" --dir="path/to/directory/you/want/to/scan" --file-ext=".scan_only_files_with_this_extension" --log`

### How do I build it?
You only need CMAKE and MAKE, from there, everything is handled for you. Just run
`bash scripts/build.sh` and it will compile Smart Find.
If you also want to add it to path, just run `bash scripts/add_to_path.sh`
and it will prompt you for the password (it's using sudo -S).
