
#ifdef EXPECT_FAIL_1

template<typename T>
class SmallVectorTemplateCommon {
protected:
  void setEnd(T *P);
};

template<typename T>
class SmallVectorImpl : public SmallVectorTemplateCommon<T> {
  T Value;
public:
  void push_back(const T& value) {
    Value = value;
    setEnd(&Value);
  }
};

void someFunc()
{
  SmallVectorImpl<int> vec;
  vec.push_back(6);
}

#endif

#ifdef EXPECT_FAIL_2

namespace N3 {
  template<typename T, typename Result>
  struct call_f0 {
    void test_f0(T t) {
      Result &result = f0(t);
    }
  };
}

template<typename T>
void templFunc(T t)
{
  short& result = f0(t);
}

short& f0(char);

void someFunc()
{
  templFunc('t');
}

#endif

#ifdef EXPECT_FAIL_3

template<typename T>
int t(T i)
{ return f(i); }

int f(int i)
{ return i; }

int main()
{
  return t(1);
}

#endif

#ifdef EXPECT_PASS_1
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=23885

class A {};

// Commenting this causes the g(argc) line below to fail to compile, but
// not the g(A()) line.  The difference is that int has no associated
// namespaces (not even the global scope).  When calling g(A()), the global
// scope is searched, finding the f(A) overload.  The global scope is not
// searched to find the f(int) overload.  That is why EXPECT_FAIL_3 is
// expected to fail, but this similar case with a non-built-in type is
// expected to pass.
template<class T>
void f(T t) {
  --t; // Statement does not compile for T = A.
}

template<class T> void g(T t) {
  f(t);
}

// Better match than f(T), but defined after the use in the arg-dependent
// use in g(T) above.
void f(A) {}
void f(int) {}

int main(int argc, char** argv) {
    g(A());
    g(argc);
    return 0;
}

#endif

#ifdef EXPECT_PASS_2
// http://gcc.gnu.org/bugzilla/show_bug.cgi?id=16635
// http://www.open-std.org/jtc1/sc22/wg21/docs/cwg_defects.html#993

template <class T>
T* foo (T* t)
{
  return t++;
}

struct aclass;

aclass*
addone(aclass* x)
{
  return foo<aclass>(x);
}

struct aclass
{
  int x;
};

void someFunc()
{
  aclass a;
  addone(&a);
}

#endif

#ifdef EXPECT_PASS_3

struct A{};

template<typename T, typename U>
void add(T t, U u)
{
  t + u;
}

template<typename T, typename U>
void templ(T t, U u)
{
  add(t, u); // ADL causes choice of N1::add.
}

namespace N1
{
  struct B{};

  void add(A, B)
  {
  }
}

void someFunc()
{
  templ(A(), N1::B());
}

#endif
