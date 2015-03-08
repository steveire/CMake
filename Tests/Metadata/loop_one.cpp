
#ifdef DEFINE_loop_one
int loop_one()
{
  return 42;
}
#endif

int main()
{
  return loop_one() - 42;
}
