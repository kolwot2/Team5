#include "ParseGraph.h"


Graph ParseGraph(std::istream& input) {
	using namespace rapidjson;
	Document doc;
	IStreamWrapper isw(input);
	doc.ParseStream(isw);
	const auto& name = doc["name"].GetString();
	const auto& ind = doc["idx"].GetInt();
	const auto& points = doc["points"].GetArray();
	const auto& lines = doc["lines"].GetArray();

	Graph graph;

	for (const auto& item : points) {
		graph.AddVertex({
			item["idx"].GetInt(),
			item["post_idx"].IsInt() ? item["post_idx"].GetInt() : -1,
			{0.0f, 0.0f},
			});
	}

	for (const auto& item : lines) {
		const auto& v = item["points"].GetArray();
		int idx = item["idx"].GetInt();
		int length = item["length"].GetInt();
		int v1 = v[0].GetInt();
		int v2 = v[1].GetInt();
		graph.AddEdge(
			{idx, length},
			v1,
			v2
		);
		graph.AddEdge(
			{ idx, length },
			v2,
			v1
		);
	}
	return graph;
}