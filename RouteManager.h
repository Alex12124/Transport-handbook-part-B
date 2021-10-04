#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <cmath>
#include <optional>

struct BusStats {
  int stop_count;
  int unique_stop_count;
  double route_distance;
};

struct Coords {
  long double latitude;
  long double longitude;
};

class StopDataBase {
public:
  Coords GetCoords() const{
	return coords;
  }

  void SetCoords(const Coords& coords_) {
	coords = coords_;
  }

  std::set<std::string>& GetBuses() {
  	return buses;
  }

  const std::set<std::string>& GetBuses() const {
	return buses;
  }

private:
  Coords coords;
  std::set<std::string> buses;
};

//---------------------Pattern Strategy-----------------------//
class Strategy {
public:
  virtual ~Strategy() = default;
  virtual int ComputeStopsOnRoute(const std::vector<std::string>& stops) const = 0;

  virtual double ComputeDistanceOnRoute(const std::vector<std::string>& stops,
		  const std::unordered_map<std::string, StopDataBase>& stop_db) const  = 0;

  double ComputeDistance(const Coords& lhs, const Coords& rhs) const;

  int ComputeUniqueStopsOnRoute(const std::vector<std::string>& stops) const;

  void FillBusesInStopDB(const std::vector<std::string>& stops,
		  const std:: string& bus_name,
		  std::unordered_map<std::string, StopDataBase>& stop_db) {
	for(const std::string& stop_name: stops) {
	  stop_db[stop_name].GetBuses().insert(bus_name);
	}
  }
};

class CycleStrategy : public Strategy {
public:
  int ComputeStopsOnRoute(const std::vector<std::string>& stops) const override {
    return stops.size();
  }

  double ComputeDistanceOnRoute(const std::vector<std::string>& stops,
		  const std::unordered_map<std::string, StopDataBase>& stop_db) const override {
    double sum = 0;
	for(auto it = begin(stops); it != prev(end(stops)); ++it) {
	  sum += ComputeDistance(stop_db.at(*it).GetCoords(),
			  stop_db.at(*next(it)).GetCoords());

    }
	return sum;
  }
};

class NotCycleStrategy : public Strategy {
public:
  int ComputeStopsOnRoute(const std::vector<std::string>& stops) const override {
    return stops.size() * 2 - 1;
  }

  double ComputeDistanceOnRoute(const std::vector<std::string>& stops,
		  const std::unordered_map<std::string, StopDataBase>& stop_db) const override {
	double sum = 0;
	for(auto it = begin(stops); it != prev(end(stops)); ++it) {
	  sum += 2 * ComputeDistance(stop_db.at(*it).GetCoords(),
			  stop_db.at(*next(it)).GetCoords());
	}
	return sum;
  }
};
//---------------------Pattern Strategy-----------------------//


//---------------------Business Logic of Programm----------------//
class RouteManager {
public:
  void SetStopCoords(const std::string& stop_name, Coords coords) {
	stop_db[stop_name].SetCoords(coords);
  }

  void SetBusRoute(const std::string& bus_name, const std::vector<std::string>& stops) {
	BusStats stats;
	stats.stop_count = strategy->ComputeStopsOnRoute(stops);
	stats.unique_stop_count = strategy->ComputeUniqueStopsOnRoute(stops);
	stats.route_distance = strategy->ComputeDistanceOnRoute(stops, stop_db);
    bus_stats[bus_name] = stats;
    strategy->FillBusesInStopDB(stops, bus_name, stop_db);
  }

  void SetStrategy(Strategy* strategy_) {
	strategy = strategy_;
  }

  std::optional<BusStats> GetBusStats(const std::string& bus_name) const {
	if(bus_stats.count(bus_name)) {
	  return bus_stats.at(bus_name);
	}
	return std::nullopt;
  }

  std::optional<std::set<std::string>> GetStopStats(const std::string& stop_name) {
	if(stop_db.count(stop_name)) {
	  return stop_db[stop_name].GetBuses();
	}
	return std::nullopt;

  }

private:
  std::unordered_map<std::string, BusStats> bus_stats;
  std::unordered_map<std::string, StopDataBase> stop_db;
  Strategy* strategy;
};
//---------------------Business Logic of Programm----------------//

//---------------------Tests-----------------------------------//
void TestComputeDistance();
void TestBusStats();
void TestStopStats();
//---------------------Tests-----------------------------------//
