#include "common.h"
#include "vec.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef vec_t(size_t) size_t_vec_t;
typedef vec_t(Vector2) vector_vec_t;

typedef enum {
  Ground,
  Start,
  North_South,
  East_West,
  North_East,
  North_West,
  South_East,
  South_West,
} Cell_Kind;

typedef struct {
  Cell_Kind kind;
  size_t idx;
  i32 parent;
  i32 dist;
  i32 ydir;
} Cell;

void dbg_print_map(Cell *map, size_t w, size_t h);
void dbg_map_results(Cell *map, size_t w, size_t h);

size_t find_start(Cell *map, size_t w, size_t h);
i64 walk_map(Cell *map, size_t w, size_t h, size_t start);
void check_nests(Cell *map, size_t w, size_t h, size_t start);

Vector2 cell_coord(size_t idx, size_t w);
void get_neighbours(Cell *map, size_t w, size_t h, size_t idx, i64 *neighbours);
bool in_closed_set(size_t_vec_t *set, size_t t);

int main(void) {
  size_t size = 0;
  char *data = read_entire_file("input.txt", &size);

  if (data == NULL) {
    return -1;
  }

  size_t width = 0;
  size_t height = 0;

  for (size_t i = 0; i < size; i += 1) {
    char c = data[i];

    switch (c) {
    case '\n':
      height += 1;
      break;
    case '\r':
      break;
    default:
      if (height == 0) {
        width += 1;
      }
      break;
    }
  }

  printf("w: %lld, h: %lld\n", width, height);

  Cell *map = (Cell *)malloc(sizeof(Cell) * (width * height));
  memset(map, 0, sizeof(Cell) * (width * height));

  size_t x = 0;
  size_t y = 0;
  for (size_t i = 0; i < size; i += 1) {
    char c = data[i];

    switch (c) {
    case '\n':
      y += 1;
      x = 0;
      break;
    case '\r':
      break;
    default:
      Cell_Kind kind = Ground;
      switch (c) {
      case '.':
        kind = Ground;
        break;
      case 'S':
        kind = Start;
        break;
      case '|':
        kind = North_South;
        break;
      case '-':
        kind = East_West;
        break;
      case 'L':
        kind = North_East;
        break;
      case 'J':
        kind = North_West;
        break;
      case '7':
        kind = South_West;
        break;
      case 'F':
        kind = South_East;
        break;
      }
      size_t idx = y * width + x;
      map[idx] =
          (Cell){.kind = kind, .idx = idx, .dist = 0, .parent = -1, .ydir = 0};

      x += 1;
      break;
    }
  }

  size_t start = find_start(map, width, height);

  // i64 max = walk_map(map, width, height, start);

  // printf("Max distance: %ld\n", max);

  check_nests(map, width, height, start);

  free(data);
  return 0;
}

void dbg_print_map(Cell *map, size_t w, size_t h) {
  for (size_t y = 0; y < h; y += 1) {
    for (size_t x = 0; x < w; x += 1) {
      size_t idx = y * w + x;

      char *str;
      switch (map[idx].kind) {
      case Ground:
        str = "G ";
        break;
      case Start:
        str = "S ";
        break;
      case North_South:
        str = "NS";
        break;
      case East_West:
        str = "EW";
        break;
      case North_East:
        str = "NE";
        break;
      case North_West:
        str = "NW";
        break;
      case South_East:
        str = "SE";
        break;
      case South_West:
        str = "SW";
        break;
      }
      printf("%s", str);
    }

    printf("\n");
  }
}

void dbg_map_results(Cell *map, size_t w, size_t h) {
  for (size_t y = 0; y < h; y += 1) {
    for (size_t x = 0; x < w; x += 1) {
      size_t idx = y * w + x;
      if (map[idx].ydir < 10) {
        printf(" %d", map[idx].ydir);
      } else {
        printf("%d", map[idx].ydir);
      }
    }
    printf("\n");
  }
}

size_t find_start(Cell *map, size_t w, size_t h) {
  for (size_t y = 0; y < h; y += 1) {
    for (size_t x = 0; x < w; x += 1) {
      size_t idx = y * w + x;
      if (map[idx].kind == Start) {
        return idx;
      }
    }
  }

  return -1;
}

i64 walk_map(Cell *map, size_t w, size_t h, size_t start) {
  size_t_vec_t stack;
  size_t_vec_t closed_set;
  vec_init(&stack);
  vec_init(&closed_set);

  vec_insert(&stack, 0, start);

  i64 max_dist = -1;
  while (stack.length > 0) {
    size_t idx = vec_pop(&stack);
    i64 neighbours[4];

    if (!in_closed_set(&closed_set, idx)) {
      vec_push(&closed_set, idx);
    }

    max_dist = map[idx].dist > max_dist ? map[idx].dist : max_dist;
    // Get the neighbours
    get_neighbours(map, w, h, idx, &neighbours[0]);
    // Push the neighbours
    for (size_t i = 0; i < 4; i += 1) {
      i64 n = neighbours[i];
      if (n == -1) {
        continue;
      }

      size_t n_idx = (size_t)n;
      if (in_closed_set(&closed_set, n_idx)) {
        continue;
      }
      map[n_idx].dist = map[idx].dist + 1;
      vec_insert(&stack, 0, n_idx);
      vec_push(&closed_set, n_idx);
    }
  }

  vec_deinit(&stack);
  vec_deinit(&closed_set);

  return max_dist;
}

void check_nests(Cell *map, size_t w, size_t h, size_t start) {
  size_t_vec_t stack;
  size_t_vec_t closed_set;
  vec_init(&stack);
  vec_init(&closed_set);

  vec_push(&stack, start);

  map[start].parent = -1;

  size_t end = 0;
  while (stack.length > 0) {
    size_t idx = vec_pop(&stack);
    i64 neighbours[4];

    if (!in_closed_set(&closed_set, idx)) {
      vec_push(&closed_set, idx);
    }

    get_neighbours(map, w, h, idx, &neighbours[0]);
    for (size_t i = 0; i < 4; i += 1) {
      i64 n = neighbours[i];
      if (n == -1) {
        continue;
      }

      size_t n_idx = (size_t)n;
      if (in_closed_set(&closed_set, n_idx)) {
        continue;
      }
      map[n_idx].dist = map[idx].dist + 1;
      map[n_idx].parent = idx;
      vec_push(&stack, n_idx);
      // vec_push(&closed_set, n_idx);
    }
    end = idx;
  }

  Vector2 coord = cell_coord(end, w);
  // printf("End: [%f, %f]\n", coord.x, coord.y);

  size_t current = end;
  Vector2 first_coord = cell_coord(end, w);
  for (;;) {
    Cell *c = &map[current];
    Vector2 coord = cell_coord(c->idx, w);

    if (c->parent == -1) {
      i32 ydir = ydir = (i32)(coord.y - first_coord.y);
      switch (c->kind) {
      case North_East:
        c->ydir = ydir == 0 ? -1 : 1;
        break;
      case North_West:
        c->ydir = ydir == 0 ? -1 : 1;
        break;
      case South_East:
        c->ydir = ydir == 0 ? 1 : -1;
        break;
      case South_West:
        c->ydir = ydir == 0 ? 1 : -1;
        break;
      default:
        c->ydir = ydir;
        break;
      }
      break;
    }

    Vector2 parent_coord = cell_coord(c->parent, w);
    i32 ydir = ydir = (i32)(coord.y - parent_coord.y);
    switch (c->kind) {
    case North_East:
      c->ydir = ydir == 0 ? -1 : 1;
      break;
    case North_West:
      c->ydir = ydir == 0 ? -1 : 1;
      break;
    case South_East:
      c->ydir = ydir == 0 ? 1 : -1;
      break;
    case South_West:
      c->ydir = ydir == 0 ? 1 : -1;
      break;
    default:
      c->ydir = ydir;
      break;
    }

    current = c->parent;
  }

  size_t sum = 0;
  for (size_t y = 0; y < h; y += 1) {
    // i32 wall_count = 0;
    i32 first_ydir = 0;
    bool isNest = false;
    for (size_t x = 0; x < w; x += 1) {
      size_t idx = y * w + x;
      Cell *c = &map[idx];

      if (c->ydir != 0 && first_ydir == 0) {
        first_ydir = c->ydir;
      }

      if (c->ydir == 1) {
        isNest = true;
      } else if (c->ydir == -1) {
        isNest = false;
      }

      if (c->kind == Ground && isNest) {
        Vector2 coord = cell_coord(c->idx, w);
        sum += 1;
      }

      // if (c->ydir != 0) {
      //   wall_count += 1;
      // }

      // if (c->kind == Ground && wall_count % 2 == 0) {
      //   Vector2 coord = cell_coord(c->idx, w);
      //   sum += 1;
      //   printf("Nest at [%f, %f] - Wall Count %d\n", coord.x, coord.y,
      //          wall_count);
      // }
    }
  }

  printf("Total: %lld\n", sum);
}

Vector2 cell_coord(size_t idx, size_t w) {
  return (Vector2){.x = (f32)(idx % w), .y = (f32)(idx / w)};
}

void get_neighbours(Cell *map, size_t w, size_t h, size_t idx,
                    i64 *neighbours) {
  Cell cell = map[idx];
  size_t x = idx % w;
  size_t y = idx / w;

  memset(neighbours, -1, sizeof(i64) * 4);
  switch (cell.kind) {
  case Ground:
    break;
  case Start:
    if (y - 1 >= 0) {
      Cell_Kind k = map[idx - w].kind;
      if (k == South_East || k == South_West || k == North_South) {
        neighbours[0] = idx - w;
      }
    }
    if (y + 1 < h) {
      Cell_Kind k = map[idx + w].kind;
      if (k == North_East || k == North_West || k == North_South) {
        neighbours[1] = idx + w;
      }
    }
    if (x - 1 >= 0) {
      Cell_Kind k = map[idx - 1].kind;
      if (k == East_West || k == North_East || k == South_East) {
        neighbours[2] = idx - 1;
      }
    }

    if (x + 1 >= 0) {
      Cell_Kind k = map[idx + 1].kind;
      if (k == East_West || k == North_West || k == South_West) {
        neighbours[3] = idx + 1;
      }
    }
    break;
  case North_South:
    if (y - 1 >= 0) {
      neighbours[0] = idx - w;
    }
    if (y + 1 < h) {
      neighbours[1] = idx + w;
    }
    break;
  case East_West:
    if (x - 1 >= 0) {
      neighbours[0] = idx - 1;
    }
    if (x + 1 < w) {
      neighbours[1] = idx + 1;
    }
    break;
  case North_East:
    if (y - 1 >= 0) {
      neighbours[0] = idx - w;
    }
    if (x + 1 < w) {
      neighbours[1] = idx + 1;
    }
    break;
  case North_West:
    if (y - 1 >= 0) {
      neighbours[0] = idx - w;
    }
    if (x - 1 >= 0) {
      neighbours[1] = idx - 1;
    }
    break;
  case South_East:
    if (y + 1 < h) {
      neighbours[0] = idx + w;
    }
    if (x + 1 < w) {
      neighbours[1] = idx + 1;
    }
    break;
  case South_West:
    if (y + 1 < h) {
      neighbours[0] = idx + w;
    }
    if (x - 1 >= 0) {
      neighbours[1] = idx - 1;
    }
    break;
  }
}

bool in_closed_set(size_t_vec_t *set, size_t t) {
  i32 i;
  size_t val;

  vec_foreach(set, val, i) {
    if (val == t) {
      return true;
    }
  }

  return false;
}