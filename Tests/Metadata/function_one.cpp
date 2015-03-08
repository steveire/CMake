
#ifdef DEFINE_function_one
int function_one()
{
  return 42;
}
#endif

int main()
{
  return function_one() - 42;
}
