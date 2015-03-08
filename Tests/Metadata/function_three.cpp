
#ifdef DEFINE_function_three
int function_three()
{
  return 42;
}
#endif

int main()
{
  return function_three() - 42;
}
