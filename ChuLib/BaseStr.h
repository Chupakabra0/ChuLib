#pragma once
#include <iostream>
#include <cstring>

#define DEBUG

namespace Chu {
	template<class Type>
	concept IsStringable = std::is_same_v<Type, char> || std::is_same_v<Type, wchar_t> || std::is_same_v<Type, char8_t> || std::is_same_v<Type, char16_t> || std::is_same_v<Type, char32_t>;
							
	template<IsStringable Type, size_t MaxStaticSize>
	class BaseStr {
		using value_type = Type;
		using size_type = size_t;
		using array = value_type[];
		using const_array = const array;
		using pointer = value_type*;
		using const_pointer = const pointer;
		using reference = value_type&;
		using const_reference = const reference;
		using difference_type = std::ptrdiff_t;
	public:

		BaseStr() {
			this->size = 1u;
			this->capacity = MaxStaticSize;
			this->data.staticData[this->GetSize()] = '\0';
		}

		BaseStr(const_array str) {
			this->size = strlen(str) + 1;
			if (this->size > MaxStaticSize) {
				this->capacity = this->size * 2;
				this->data.dynamicData = Allocate(this->capacity);
			}
			else {
				this->capacity = MaxStaticSize;
			}

			for (auto i = 0u; i < this->GetSize(); ++i) {
				(*this)[i] = str[i];
			}
			(*this)[this->GetSize()] = '\0';
		}

		BaseStr(const value_type symbol, const size_type count = 1u) {
			this->size = count + 1;
			if (this->size > MaxStaticSize) {
				this->capacity = this->size * 2;
				this->data.dynamicData = Allocate(this->capacity);
			}
			else {
				this->capacity = MaxStaticSize;
			}

			for (auto i = 0u; i < this->GetSize(); ++i) {
				(*this)[i] = symbol;
			}
			(*this)[this->GetSize()] = '\0';
		}

		BaseStr(const BaseStr& copy) noexcept {
			this->size = copy.GetSize() + 1;
			if (this->size > MaxStaticSize) {
				this->capacity = this->size * 2;
				this->data.dynamicData = Allocate(this->capacity);
			}
			else {
				this->capacity = MaxStaticSize;
			}

			for (auto i = 0u; i < this->GetSize(); ++i) {
				(*this)[i] = copy[i];
			}
			(*this)[this->GetSize()] = '\0';
		}

		BaseStr(BaseStr&& move) noexcept {
			this->size = move.GetSize() + 1;
			this->capacity = move.capacity;
			this->data = move.data;

			move.size = 1u;
			move.capacity = MaxStaticSize;
			move.data.dynamicData = nullptr;
		}

		~BaseStr() {
			this->Delete();
		}

		[[nodiscard]] size_type GetSize() const {
			return this->size - 1u;
		}

		[[nodiscard]] size_type GetCapacity() const {
			return this->capacity;
		}

		[[nodiscard]] const value_type* GetCString() const {
			if (this->IsStatic()) {
				return this->data.staticData;
			}
			return this->data.dynamicData;
		}

		friend constexpr bool operator==(const BaseStr& first, const BaseStr& second) {
			if (first.size != second.size) return false;

			for (auto i = 0u; i < first.size; ++i) {
				if (first[i] != second[i]) {
					return false;
				}
			}
			return true;
		}

		friend constexpr bool operator!=(const BaseStr& first, const BaseStr& second) {
			return !(first == second);
		}
		
	private:
		
		union StrData {
			value_type staticData[MaxStaticSize];
			pointer    dynamicData;
		} data;

		size_type size;
		size_type capacity;

		static pointer Allocate(const size_type capacity) {
			if (capacity > 0u) {
				return new value_type[capacity];
			}
			return nullptr;
		}

		void Delete() {
			if (!this->IsStatic()) {
				delete[] this->data.dynamicData;
			}
			this->capacity = MaxStaticSize;
			this->size = 0u;
		}

		[[nodiscard]] constexpr bool IsStatic() const {
			return MaxStaticSize == this->capacity;
		}
	};

	using AsciiStr    = BaseStr<char, 24u>;
	using UTF8Str     = BaseStr<char8_t, 24u>;
	using UTF16Str    = BaseStr<char16_t, 24u>;
	using UTF32Str    = BaseStr<char32_t, 24u>;
	using UnicodeStr  = BaseStr<wchar_t, 24u>;
}