import random
import csv

def generate_grid(width, height, mode, output_cfg, output_csv, temp=None, rectangles=[]):
    """
    Generates a temperature grid based on mode and rectangle definitions.

    Args:
        width: Integer representing the grid width.
        height: Integer representing the grid height.
        mode: String, either "R" (random) or "U" (uniform).
        min_temp: Integer representing the minimum temperature for random mode (optional).
        max_temp: Integer representing the maximum temperature for random mode (optional).
        uniform_temp: Integer representing the uniform temperature for uniform mode (optional).
        rectangles: List of lists, where each inner list defines a rectangle [x0, y0, x1, y1, temp].

    Returns:
        Nothing
    """
    grid = [[0 for _ in range(width)] for _ in range(height)]

    # Fill based on mode
    if mode == "R":
        tempParams = temp.split('/')
        min_temp = int(tempParams[0])
        max_temp = int(tempParams[1])

        for i in range(height):
            for j in range(width):
                grid[i][j] = random.randint(min_temp, max_temp)

    elif mode == "U":
        for i in range(height):
            for j in range(width):
                grid[i][j] = float(temp)
    else:
        raise ValueError("Invalid mode. Supported modes are 'R' and 'U'.")

    with open(output_cfg, 'a', newline='',  encoding='utf-8') as cfgfile:
        for rect in rectangles:
            print("Rect: ",rect)
            x0, y0, x1, y1, temp = rect
            cfgfile.write(f"constrect={x0}/{y0}/{x1}/{y1}\n")
            for i in range(y0, y1 + 1):
                for j in range(x0, x1 + 1):  # Decrement x to ensure proper range
                    grid[i][j] = float(temp)

        cfgfile.write(f"platepath={output_csv}\n")
        cfgfile.write(f"width={width}\n")
        cfgfile.write(f"height={height}\n")

    write_grid_to_csv(grid, output_csv)
def write_grid_to_csv(grid, filename):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(grid)

def main():

    import argparse

    parser = argparse.ArgumentParser(description="Generate temperature grid CSV file.")
    parser.add_argument("width", type=int, help="Grid width")
    parser.add_argument("height", type=int, help="Grid height")
    parser.add_argument("output_csv", help="Output CSV filename")
    parser.add_argument("output_cfg", help="Ouput configuration")
    parser.add_argument("mode", type=str,  help="Filling mode (R: random, U: uniform)")
    parser.add_argument("temp", type=str, help="Uniform temperature or min/max random temperature bounds")
    parser.add_argument("rectangles", nargs="*", help="Rectangles boundaries and temperature of format: x0 y0 x1 y1 temp")

    args = parser.parse_args()

    rectangles = []
    for i in range(0, len(args.rectangles), 5):
        newRect = []
        for j in range(5):
            newRect.append(int(args.rectangles[i+j]))
        print(newRect)
        rectangles.append(newRect)
    print(rectangles)

    generate_grid(args.width, args.height, args.mode, args.output_cfg, args.output_csv, args.temp, rectangles)

if __name__ == "__main__":
    main()