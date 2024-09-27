#pragma once
#include <cstdint>
#include <vector>
#include<stdint.h>
#include"MathUtilityForText.h"
#include"model.h"
#include "WorldTransform.h"



//MapChipFieldの上に書く
enum  MapChipType{
	kBlank,
	kBlock,
   

};
struct  MapChipData {
	std::vector<std::vector<MapChipType>>data;
};



class MapChipField{
public:
	//1ブロックのサイズ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	struct IndexSet {

		uint32_t xIndex;
		uint32_t yIndex;
	};
	struct Rect{
		float left;
		float right;
		float bottom;
		float top;
	};
	
	void ResetMapChipData();
	void LoadMapChipCsv(const std::string& filePath);
	 Vector3 GetMapChipPositionByIndex(uint32_t xIndex,uint32_t yIndex);
	 MapChipType GetMapChipTypeByIndex(uint32_t xIndex,uint32_t yIndex);
	 MapChipType GetMapChipTypeByPosition(const Vector3& position);
	 IndexSet GetMapChipIndexSetByPosition(const Vector3& position);
	 int32_t GetNumBlockVirtical()const { return kNumBlockVirtical; }
	 uint32_t GetNumBlockHorizontal()const { return kNumBlockHorizontal; }
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

private:

	

	//ブロックの個数
	static inline  const uint32_t kNumBlockVirtical = 30;
	static inline  const uint32_t kNumBlockHorizontal=100;
	MapChipData mapChipData_;

	
};