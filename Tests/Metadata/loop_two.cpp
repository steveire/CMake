
#ifdef DEFINE_loop_two
int loop_two()
{
  return 42;
}
#endif

int main()
{
  return loop_two() - 42;
}
