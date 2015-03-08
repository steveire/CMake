
#ifdef DEFINE_function_two
int function_two()
{
  return 42;
}
#endif

int main()
{
  return function_two() - 42;
}
