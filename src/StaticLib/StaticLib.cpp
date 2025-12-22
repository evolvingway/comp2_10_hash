#define WIN32_LEAN_AND_MEAN             // Windows ヘッダーからほとんど使用されていない部分を除外する
#include "Windows.h"                    // Windows API の機能定義
#include <stdlib.h>
#include <string.h>

#include "../include/lib_func.h"


static unsigned int hash_func(unsigned int key, int num)
{
	return key % num;
}

static unsigned int get_hash(const hash* h, unsigned int key)
{
	if (h == NULL) return ~0;
	if (key == ~0) return ~0;

	unsigned int base = hash_func(key, h->max_size);

	// オープンアドレス法（線形探索）
	for (unsigned int i = 0; i < h->max_size; i++) {
		unsigned int idx = (base + i) % h->max_size;

		// 未使用、または同じキーが見つかったらそこを返す
		if (h->nodes[idx].key == ~0 || h->nodes[idx].key == key) {
			return idx;
		}
	}

	return ~0;
}

// ハッシュの初期化 (max_size は ~0 未満)
bool initialize(hash* h, unsigned int max_size)
{
	if (h == NULL) return false;
	if (max_size == 0 || max_size == ~0) return false;

	if (h->nodes != NULL) finalize(h);

	h->nodes = (hash_node*)malloc(max_size * sizeof(hash_node));
	if (h->nodes == NULL) return false;

	for (unsigned int i = 0; i < max_size; i++) {
		h->nodes[i].key = ~0;
		h->nodes[i].value[0] = '\0';
	}

	h->max_size = max_size;
	return true;
}

// 使用メモリの全解放
void finalize(hash* h)
{
	if (h == NULL) return;

	free(h->nodes);
	h->nodes = NULL;
	h->max_size = 0;
}

// key の値を見てノードを追加する（追加できなかったら false）
bool add(hash* h, unsigned int key, const char* value)
{
	if (h == NULL) return false;
	if (h->max_size == ~0) return false;
	if (key == ~0) return false;

	unsigned int idx = get_hash(h, key);
	if (idx == ~0) return false;

	// すでに使用中なら追加失敗（上書きしない仕様）
	if (h->nodes[idx].key != ~0) return false;

	h->nodes[idx].key = key;
	strcpy_s(
		h->nodes[idx].value,
		sizeof(h->nodes[idx].value),
		value
	);

	return true;
}

// key の値を見てノードを検索し、値を取得する（なければ NULL）
const char* get(const hash* h, unsigned int key)
{
	if (h == NULL) return NULL;
	if (key == ~0) return NULL;

	unsigned int base = hash_func(key, h->max_size);

	for (unsigned int i = 0; i < h->max_size; i++) {
		unsigned int idx = (base + i) % h->max_size;

		// 未使用スロットに到達したら探索終了
		if (h->nodes[idx].key == ~0) {
			return NULL;
		}

		if (h->nodes[idx].key == key) {
			return h->nodes[idx].value;
		}
	}

	return NULL;
}

// ハッシュの値を取得する（デバッグ用）
int debug_get_hash(const hash* h, unsigned int key)
{
	return get_hash(h, key);
}
