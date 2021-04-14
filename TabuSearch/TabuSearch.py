from data_parser import DataParser


def main():
    parser = DataParser("I1.txt")
    print(parser.get_data_frame())


if __name__ == '__main__':
    main()
