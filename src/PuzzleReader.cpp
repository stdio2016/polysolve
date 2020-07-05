#include <exception>
#include "PuzzleReader.hpp"
#include <string>

using json = nlohmann::json;

Puzzle PuzzleReader::read(std::istream &is) {
  json j;
  is >> j;
  return j.get<Puzzle>();
}

void from_json(const json &j, Puzzle &puzzle) {
  if (!j.is_object()) {
    throw std::runtime_error("Puzzle file must be a JSON of object.");
  }
  std::string grid = j.at("grid").get<std::string>();
  puzzle.grid = GridType::fromName(grid);
  if (!puzzle.grid) {
    throw std::runtime_error("Undefined or unimplemented grid: " + grid);
  }
  puzzle.board = j.at("board").get<std::vector<Coord> >();
}

void readTile1D(const json &j, std::vector<Coord> &coord, int y, int z, int w) {
  if (!j.is_array()) {
    throw std::runtime_error("Not regular shaped multidimentional array");
  }
  for (int x = 0; x < j.size(); x++) {
    const json &tile = j[x];
    if (tile.get<int>() > 0) {
      coord.push_back(Coord{x, y, z, w});
    }
  }
}

void readTile2D(const json &j, std::vector<Coord> &coord, int z, int w) {
  if (!j.is_array()) {
    throw std::runtime_error("Not regular shaped multidimentional array");
  }
  for (int y = 0; y < j.size(); y++) {
    readTile1D(j[y], coord, y, z, w);
  }
}

void readTile3D(const json &j, std::vector<Coord> &coord, int w) {
  if (!j.is_array()) {
    throw std::runtime_error("Not regular shaped multidimentional array");
  }
  for (int z = 0; z < j.size(); z++) {
    readTile2D(j[z], coord, z, w);
  }
}

void readTile4D(const json &j, std::vector<Coord> &coord) {
  if (!j.is_array()) {
    throw std::runtime_error("Not regular shaped multidimentional array");
  }
  for (int w = 0; w < j.size(); w++) {
    readTile3D(j[w], coord, w);
  }
}

void from_json(const json &j, std::vector<Coord> &coord) {
  const char *shapeError = "Shape must be an object with either array of coords or multidimensional array of tiles";
  if (!j.is_object()) {
    throw std::runtime_error(shapeError);
  }
  int finish = 0;
  if (j.contains("tiles")) {
    if (!j["tiles"].is_array()) {
      throw std::runtime_error(shapeError);
    }
    int dim = 0;
    json jj = j["tiles"];
    while (jj.is_array()) {
      dim += 1;
      jj = jj[0];
    }
    if (dim == 1) readTile1D(j["tiles"], coord, 0, 0, 0);
    else if (dim == 2) readTile2D(j["tiles"], coord, 0, 0);
    else if (dim == 3) readTile3D(j["tiles"], coord, 0);
    else if (dim == 4) readTile4D(j["tiles"], coord);
    else {
      throw std::runtime_error("Too many dimensions");
    }
    finish = 1;
  }
  if (j.contains("coords")) {
    if (finish || !j["coords"].is_array()) {
      throw std::runtime_error(shapeError);
    }
    for (json c : j["coords"]) {
      coord.push_back(c.get<Coord>());
    }
    finish = 1;
  }
  if (!finish) {
    throw std::runtime_error(shapeError);
  }
}

void from_json(const json &j, Coord &coord) {
  if (!j.is_array()) {
    throw std::runtime_error("Coord must be an integer array.");
  }
  int dim = 0;
  while (dim < j.size() && j[dim].is_number()) {
    dim += 1;
  }
  coord = Coord{0, 0, 0, 0};
  if (dim > 0) coord.x = j[0].get<int>();
  if (dim > 1) coord.y = j[1].get<int>();
  if (dim > 2) coord.z = j[2].get<int>();
  if (dim > 3) coord.w = j[3].get<int>();
}
