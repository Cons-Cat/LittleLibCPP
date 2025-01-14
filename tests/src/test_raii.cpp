#include <cat/string>
#include <cat/unique>
#include <cat/utility>

#include "../unit_tests.hpp"

inline constinit int4 raii_counter = 0;

struct type {
   cat::string data;

   type() = default;

   type(cat::string string) : data(mov string) {
      // cat::print(data);
      // cat::println(" constructor");
   }

   // NOLINTNEXTLINE This is a non-trivial destructor.
   ~type() {
      // cat::print("~");
      // cat::println(this->data);
   }

   auto
   operator=(cat::string string) -> type& {
      this->data = mov string;
      return *this;
   }

   void
   raii() const {
      // cat::print(this->data);
      // cat::println(" calls raii()!");
      ++raii_counter;
   }
};

// NOLINTNEXTLINE
void pass_by_value(cat::unique_weak<type>){};

test(raii) {
   // TODO: Fix `unique` and re-enable these tests.
   // cat::println("Construct objects.");
   // Test constructor.
   cat::unique_weak<type> foo(cat::string("foo"));
   // Test assignment.
   foo = cat::string("foo");
   cat::verify(foo.has_ownership());

   cat::unique_weak<type> moo(cat::string("moo"));
   cat::verify(moo.has_ownership());

   // Test move-assignment.
   // cat::println("Move moo into foo.");
   foo = mov moo;
   cat::verify(!moo.has_ownership());

   // cat::println("Move foo into func().");
   // A move is required:
   pass_by_value(mov foo);
   cat::verify(!foo.has_ownership());

   // This is deliberately ill-formed:
   // func(foo);

   // Default construct `	cat::unique<Foo>`.
   cat::unique_weak<type> goo;
   cat::verify(goo.has_ownership());
   // Extract goo.
   auto _ = goo.borrow();
   cat::verify(!goo.has_ownership());

   // `raii()` should have been called exactly three times.
   cat::verify(raii_counter == 3);

   // Deduction guides should work here.
   cat::unique_weak weak = 1;
   cat::unique unique = weak.borrow();

   // Borrowing `weak`'s data makes it lose ownership.
   cat::verify(!weak.has_ownership());
   weak = 2;
   cat::verify(weak.has_ownership());

   // Permanately transferring ownership a `cat::unique`'s storage is unsafe,
   // but possible:
   weak = unique.borrow();
   cat::verify(weak.has_ownership());

   // `cat::unique` can be assigned over, which will call its old data's
   // destructor.
   unique = 2;

   cat::unique<int> original = 0;
   cat::unique<int8> const into = mov original;
}
