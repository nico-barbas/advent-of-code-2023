def is_number(c):
    return c >= "0" and c <= "9"


def check_spelled_number(str, c):
    ok = False
    result = 0
    match c:
        case "o":
            if check_sequence(str[1:], "ne"):
                print("It's a one!")
                ok = True
                result = 1
        case "t":
            if check_sequence(str[1:], "wo"):
                print("It's a two!")
                ok = True
                result = 2
            elif check_sequence(str[1:], "hree"):
                print("It's a three!")
                ok = True
                result = 3
        case "f":
            if check_sequence(str[1:], "our"):
                print("It's a four!")
                ok = True
                result = 4
            elif check_sequence(str[1:], "ive"):
                print("It's a five!")
                ok = True
                result = 5
        case "s":
            if check_sequence(str[1:], "ix"):
                print("It's a six!")
                ok = True
                result = 6
            elif check_sequence(str[1:], "even"):
                print("It's a seven!")
                ok = True
                result = 7
        case "e":
            if check_sequence(str[1:], "ight"):
                print("It's a eight!")
                ok = True
                result = 8
        case "n":
            if check_sequence(str[1:], "ine"):
                print("It's a nine!")
                ok = True
                result = 9

    return ok, result


def check_sequence(str: str, seq: str):
    if len(str) < len(seq):
        return False
    for i, c in enumerate(seq):
        if str[i] != c:
            return False
    return True


def parse_line(line):
    result = 0
    first = None
    last = None
    for i, c in enumerate(line):
        if is_number(c):
            if first is None:
                first = int(c, 10)
            last = int(c, 10)
        else:
            ok, num = check_spelled_number(line[i:], c)
            if ok:
                if first is None:
                    first = num
                last = num
    if not (first is None and last is None):
        print(str(first))
        result = int(str(first) + str(last), 10)
    return result


with open("input.text", "r") as input_file:
    input = input_file.read()
    lines = input.split("\n")
    acc = 0
    for line in lines:
        acc += parse_line(line)

    print(acc)
