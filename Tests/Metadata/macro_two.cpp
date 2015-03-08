
#ifdef DEFINE_macro_two
int macro_two()
{
  return 42;
}
#endif

int main()
{
  return macro_two() - 42;
}
