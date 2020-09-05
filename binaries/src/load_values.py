def tokenize(str, token):
    tokenized = ""
    for c in str:
        if c == token:
            return tokenized
        tokenized += c
    return str


def load_values(filename):
    file = open(filename, "r")
    lines = file.readlines()
    file.close()

    values = []

    i = 0
    for line in lines:
        if line != "\n":
            values.append([])
            line = line.replace('\n', '')
            while len(line) > 0:
                rgb = tokenize(line, ';')
                line = line[len(rgb) + 1:]
                rgb = rgb[1:]
                rgb = rgb[:len(rgb) - 1]

                r = tokenize(rgb, ',')
                rgb = rgb[len(r) + 1:]
                g = tokenize(rgb, ',')
                rgb = rgb[len(g) + 1:]
                b = tokenize(rgb, ',')
                rgb = rgb[len(b) + 1:]
                a = tokenize(rgb, ',')

                values[i].append((int(r), int(g), int(b), int(a)))
            i += 1
    return values


if __name__ == "__main__":
    v = load_values("okay.pd")
    print(len(v), len(v[0]))
    for l in v:
        print(l)