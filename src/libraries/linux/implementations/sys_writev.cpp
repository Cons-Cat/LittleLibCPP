#include <cat/linux>

auto
nix::sys_writev(nix::file_descriptor file_descriptor,
                cat::span<nix::io_vector> const& vectors)
   -> nix::scaredy_nix<cat::idx> {
   return nix::syscall<cat::idx>(20, file_descriptor, vectors.data(),
                                 vectors.size());
}
