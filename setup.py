from distutils.core import setup, Extension


def main():
    setup(name="simple_graphs",
          version="1.0.0",
          description="moudul simple_graphs",
          author="<Kacper>",
          author_email="example@example.com",
          ext_modules=[Extension("simple_graphs", ["simple_graphsmodule.c"])])


if __name__ == '__main__':

    main()