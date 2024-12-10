#include <cat/array>
#include <cat/linux>
#include <cat/memory>
#include <cat/socket>
#include <cat/string>

auto
main() -> int {
   cat::socket_unix<cat::socket_type::stream> listening_socket;
   // A leading null byte puts this path in the abstract namespace.
   listening_socket.path_name = cat::make_str_inplace<108>("\0/tmp/temp.sock");
   listening_socket.create().or_exit();
   listening_socket.bind().or_exit();
   listening_socket.listen(20).or_exit();

   cat::socket_unix<cat::socket_type::stream> recieving_socket;
   cat::str_inplace<12> message_buffer;

   bool exit = false;
   while (!exit) {
      recieving_socket.accept(listening_socket).or_exit();

      while (true) {
         cat::iword message_length =
            recieving_socket
               .recieve(message_buffer.data(), message_buffer.size())
               .or_exit();

         cat::str_view const input = {message_buffer.data(),
                                      message_buffer.size()};

         // TODO: This comparison is always false.
         if (cat::compare_strings(input, "exit")) {
            auto _ = cat::println("Exiting.");
            exit = true;
            break;
         }

         // Zero out the message buffer's ending.
         for (cat::iword i = message_length; i < input.size(); ++i) {
            message_buffer[cat::idx(i)] = '\0';
         }

         auto _ = cat::print("Recieved: ");
         auto _ = cat::println(message_buffer);
         break;
      }
   }

   recieving_socket.close().verify();
   listening_socket.close().verify();
   auto _ = nix::sys_unlink(listening_socket.path_name.data()).verify();
}
