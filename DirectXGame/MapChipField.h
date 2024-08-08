#include"Vector3.h"
#include <cstdint>
#include <string>
#include <vector>

// 伝統的な列挙と比べてenum classには次の利点があります。
// enum classの列挙値は暗黙的に整数型に変換されず、
// 明示的に型変換を指定する必要があります。
// enum classのenum値はそのenum型内で一意であり、
// 他のスコープの識別子とは衝突しません
enum class MapChipType {
	kBlank, // 空白
	kBlock, // ブロック
};


struct MapChipData {
	std::vector<std::vector<MapChipType>> data;
};

/// <summary>
/// マップチップフィールド
/// </summary>
class MapChipField {
public:
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;

	void ResetMapChipData();

	void LoadMapChipCsv(const std::string& filePath);

	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);

	MapChipType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);

	uint32_t GetNumBlockVirtical() const { return kNumBlockVirtical; }

	uint32_t GetNumBlockHorizontal() const { return kNumBlockHorizontal; }

private:
	static inline const uint32_t kNumBlockVirtical = 20;
	static inline const uint32_t kNumBlockHorizontal = 100;

	MapChipData mapChipData_;
};
