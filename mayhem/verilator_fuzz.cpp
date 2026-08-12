// mayhem/verilator_fuzz.cpp — file-input launcher for verilator_bin (Mayhem smoketest compat).
//
// Mayhem's file-input smoketest (393831) requires an observable read of the staged @@ path.
// verilator_bin reads via C++ streams in ways the smoketest does not attribute to the input
// file when flags precede the path. This tiny ELF opens argv[1] explicitly, then execv()s the
// sanitized front-end with the same arguments test.sh uses (file last). execv REPLACES the
// process image (same PID, no fork), so Mayhem's ptrace-based coverage collection follows
// straight into the instrumented verilator_bin.
//
// SCRATCH MUST LIVE UNDER /dev/shm.  During coverage collection Mayhem mounts the whole image
// read-only — /mayhem, the cwd AND /tmp are all read-only; the only writable location is the
// default /dev/shm tmpfs.  The first version of this launcher passed `--Mdir /tmp
// --json-only-output /tmp/verilator.tree.json`, so verilator aborted with
// "%Error: Can't write file: /tmp/verilator.tree.json" on EVERY input and the run recorded
// edges_covered=0 while still "succeeding" (the smoketest only checks the read above).
// Reproduce with:  docker run --read-only <image> /mayhem/verilator_fuzz <seed>
//
// The Mdir is a small per-slot directory (pid % SLOTS) that is emptied on entry, so the
// tmpfs footprint stays bounded no matter how many iterations a campaign runs (verilator
// writes a V<top>.tree.meta.json into the Mdir, named after the input's top module).  The
// AST dump itself goes to /dev/null — it is never read back.
#include <dirent.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static const int SLOTS = 64;

static void empty_dir(const char* path) {
    DIR* d = opendir(path);
    if (!d) return;
    struct dirent* e;
    while ((e = readdir(d)) != nullptr) {
        if (e->d_name[0] == '.'
            && (e->d_name[1] == '\0' || (e->d_name[1] == '.' && e->d_name[2] == '\0')))
            continue;
        unlinkat(dirfd(d), e->d_name, 0);
    }
    closedir(d);
}

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    const char* input = argv[1];
    int fd = open(input, O_RDONLY);
    if (fd < 0) return 1;
    char byte;
    if (read(fd, &byte, 1) < 0) {
        close(fd);
        return 1;
    }
    close(fd);

    char mdir[64];
    snprintf(mdir, sizeof mdir, "/dev/shm/vlfz%d", (int)getpid() % SLOTS);
    mkdir(mdir, 0700);   // pre-existing is fine
    empty_dir(mdir);     // bound the tmpfs footprint across iterations / pid reuse

    const char* args[] = {
        "/mayhem/verilator_bin",
        "--json-only",
        "--Mdir", mdir,
        "--json-only-output", "/dev/null",
        input,
        nullptr,
    };
    execv("/mayhem/verilator_bin", const_cast<char* const*>(args));
    return 1;
}
