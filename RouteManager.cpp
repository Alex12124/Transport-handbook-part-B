#include "RouteManager.h"
#include <sstream>
#include "test_runner.h"
#include <iomanip>
using namespace std;

double Strategy::ComputeDistance(const Coords& lhs, const Coords& rhs) const {

  return acos(sin(lhs.latitude) * sin(rhs.latitude) +
		      cos(lhs.latitude) * cos(rhs.latitude) *
		      cos(abs(lhs.longitude - rhs.longitude))) *
		      6371000;
}

int Strategy::ComputeUniqueStopsOnRoute(const std::vector<std::string>& stops) const {
  std::set<std::string> s(begin(stops), end(stops));
  return s.size();
}

void TestComputeDistance() {
  ostringstream os;
  os.precision(6);
  CycleStrategy strategy;
  os << strategy.ComputeDistance(Coords{55.611087 * 3.1415926535 / 180,
		37.20829 * 3.1415926535 / 180}, Coords{55.595884 * 3.1415926535 / 180,
		37.209755 * 3.1415926535 / 180});

  ASSERT_EQUAL(os.str(), "1693");
}

void TestBusStats() {
  RouteManager manager;
  vector<string> stops = {"Tolstopaltsevo", "Marushkino", "Rasskazovka"};
  {

	  //--------Not cycle--------------------//

	  unique_ptr<Strategy> not_cycle = make_unique<NotCycleStrategy>();
	  manager.SetStopCoords("Tolstopaltsevo", Coords{55.611087 * 3.1415926535 / 180,
			37.20829 * 3.1415926535 / 180});
	  manager.SetStopCoords("Marushkino", Coords{55.595884 * 3.1415926535 / 180,
			37.209755 * 3.1415926535 / 180});
	  manager.SetStopCoords("Rasskazovka", Coords{55.632761 * 3.1415926535 / 180,
			37.333324 * 3.1415926535 / 180});
	  manager.SetStopCoords("Extra stop", Coords{53.632761 * 3.1415926535 / 180,
			37.333324 * 3.1415926535 / 180});

	  manager.SetStrategy(not_cycle.get());
	  manager.SetBusRoute("750", stops);
	  BusStats stats = *manager.GetBusStats("750");
	  ASSERT_EQUAL(stats.stop_count, 3);
	  ASSERT_EQUAL(stats.unique_stop_count, 3);
	  ostringstream os;
	  os << setprecision(6);
	  os << stats.route_distance;
	  ASSERT_EQUAL(os.str(), "20939.5");
  }
  stops = {"Biryulyovo Zapadnoye", "Biryusinka", "Universam",
  		"Biryulyovo Tovarnaya", "Biryulyovo Passazhirskaya",
  		"Biryulyovo Zapadnoye"};
  {

	  //--------Cycle--------------------//

	  unique_ptr<Strategy> cycle = make_unique<CycleStrategy>();
	  manager.SetStopCoords("Biryulyovo Zapadnoye", Coords{55.574371 * 3.1415926535 / 180,
			37.6517 * 3.1415926535 / 180});
	  manager.SetStopCoords("Biryusinka", Coords{55.581065 * 3.1415926535 / 180,
			37.64839 * 3.1415926535 / 180});
	  manager.SetStopCoords("Universam", Coords{55.587655 * 3.1415926535 / 180,
			37.645687 * 3.1415926535 / 180});
	  manager.SetStopCoords("Biryulyovo Tovarnaya", Coords{55.592028 * 3.1415926535 / 180,
			37.653656 * 3.1415926535 / 180});
	  manager.SetStopCoords("Biryulyovo Passazhirskaya", Coords{55.580999 * 3.1415926535 / 180,
			37.659164 * 3.1415926535 / 180});
	  manager.SetStopCoords("Biryulyovo Zapadnoye", Coords{55.574371 * 3.1415926535 / 180,
			37.6517 * 3.1415926535 / 180});

	  manager.SetStrategy(cycle.get());
	  manager.SetBusRoute("256", stops);
	  BusStats stats = *manager.GetBusStats("256");
	  ASSERT_EQUAL(stats.stop_count, 6);
	  ASSERT_EQUAL(stats.unique_stop_count, 5);
	  ostringstream os;
	  os << setprecision(6);
	  os << stats.route_distance;
	  ASSERT_EQUAL(os.str(), "4371.02");
  }
  ASSERT(!manager.GetBusStats("555"));
}

void TestStopStats() {
  RouteManager manager;
  vector<string> stops = {"Tolstopaltsevo", "Marushkino", "Rasskazovka"};
  {

    //--------Not cycle--------------------//

    unique_ptr<Strategy> not_cycle = make_unique<NotCycleStrategy>();
    manager.SetStopCoords("Tolstopaltsevo", Coords{55.611087 * 3.1415926535 / 180,
		37.20829 * 3.1415926535 / 180});
    manager.SetStopCoords("Marushkino", Coords{55.595884 * 3.1415926535 / 180,
		37.209755 * 3.1415926535 / 180});
    manager.SetStopCoords("Rasskazovka", Coords{55.632761 * 3.1415926535 / 180,
		37.333324 * 3.1415926535 / 180});
    manager.SetStopCoords("Extra stop", Coords{53.632761 * 3.1415926535 / 180,
		37.333324 * 3.1415926535 / 180});

    manager.SetStrategy(not_cycle.get());
    manager.SetBusRoute("750", stops);
    ASSERT(manager.GetStopStats("Extra stop")->empty());
    ASSERT(!manager.GetStopStats("250"));
    ASSERT_EQUAL(*manager.GetStopStats("750"), set<string>({"Tolstopaltsevo",
    	"Marushkino", "Rasskazovka"}));
  }
}

