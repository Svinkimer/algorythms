// DynamicArray.h : включаемый файл для стандартных системных включаемых файлов
// или включаемые файлы для конкретного проекта.

#pragma once

#include <iostream>
#include <cassert>
#include <new>



namespace svinkimer {
	const size_t kDeaultCapacity{ 16 };

	template<typename T>
	class Array final {
	public:
		class Iterator {
		public:
			// arr_ doesn't own data, so there is no need for destructor
			Iterator(Array* arr, size_t index, bool reverse = false)
				: arr_(arr), idx_(index), reverse_(reverse) {
			}

			T& operator*() {
				return arr_->buf_[idx_];
			}

			const T& operator*() const {
				return arr_->buf_[idx_];
			}

			Iterator& operator++() {
				next();
				return *this;
			}

			Iterator operator++(int) {
				Iterator temp = *this;
				next();
				return temp;
			}

			// Must Have: get, set, next, has next
			const T& get() const {
				return arr_->buf_[idx_];
			}

			void set(const T& value) {
				arr_->buf_[idx_] = value;
			}

			void next() {
				if (reverse_) {
					if (idx_ > 0) --idx_;
					else idx_ = arr_->length_;  
				}
				else {
					++idx_;
				}
			}

			bool hasNext() const {
				if (reverse_) {
					return arr_->length_ > 0 && idx_ < arr_->length_;
				}
				else {
					return idx_ < arr_->length_;
				}
			}

			bool operator!=(const Iterator& other) const {
				return idx_ != other.idx_;
			}

		private:
			Array* arr_;
			size_t idx_;
			bool reverse_;
		};

		class ConstIterator {
		public:
			ConstIterator(const Array* arr, size_t index, bool reverse = false)
				: arr_(arr), idx_(index), reverse_(reverse) {
			}

			const T& operator*() const {
				return arr_->buf_[idx_];
			}

			ConstIterator& operator++() {
				next();
				return *this;
			}

			ConstIterator operator++(int) {
				ConstIterator temp = *this;
				next();
				return temp;
			}


			// Must have
			const T& get() const { return arr_->buf_[idx_]; }

			void next() {
				if (reverse_) {
					if (idx_ > 0) --idx_;
					else idx_ = arr_->length_;
				}
				else {
					++idx_;
				}
			}

			bool hasNext() const {
				if (reverse_) {
					return arr_->length_ > 0 && idx_ < arr_->length_;
				}
				else {
					return idx_ < arr_->length_;
				}
			}

			bool operator!=(const ConstIterator& other) const {
				return idx_ != other.idx_;
			}

		private:
			const Array* arr_;
			size_t idx_;
			bool reverse_;
		};


		Array() :
			capacity_{ kDeaultCapacity },
			length_{ 0 },
			buf_{ nullptr }
		{
			// This is raw. Don't forget to 'cook' in insert!!!
			buf_ = static_cast<T*> (std::malloc(capacity_ * sizeof(T)));
			assert(buf_ != nullptr);
		}

		Array(int capacity) :
			capacity_{ 0 },
			length_{ 0 },
			buf_{ nullptr }
		{
			//if (capacity < 0) {
			//	throw std::invalid_argument("Capacity can't be negative");
			//}
			assert(capacity >= 0);
			capacity_ = static_cast<size_t>(capacity);

			// This is raw. Don't forget to 'cook' in insert!!!
			if (capacity_ > 0) {
				buf_ = static_cast<T*> (std::malloc(capacity_ * sizeof(T)));
				assert(buf_ != nullptr);
			}
		}

		/* RULE OF FIVE
		* 1. Destructor +
		* 2. Copy Constructor +
		* 3. Move Constructor +
		* 4. Assignment Copy Operator +
		* 5. Assignment Move Operator +
		*/

		~Array() {
			for (size_t rmvIdx{ 0 }; rmvIdx < length_; rmvIdx++) {
				buf_[rmvIdx].~T();
			}

			std::free(buf_);
		}

		Array(const Array<T>& src) :
			capacity_{ src.capacity_ },
			length_{ src.length_ },
			buf_{ nullptr }
		{
			if (capacity_ == 0)
				return;

			buf_ = static_cast<T*> (malloc(capacity_ * sizeof(T)));
			assert(buf_ != nullptr);


			for (size_t cpyIdx{ 0 }; cpyIdx < length_; cpyIdx++) {
				new (buf_ + cpyIdx) T(src.buf_[cpyIdx]);
			}
		}

		Array(Array&& src) noexcept :
			buf_{ src.buf_ },
			length_{ src.length_ },
			capacity_{ src.capacity_ }
		{
			src.buf_ = nullptr;
			src.length_ = 0;
			src.capacity_ = 0;
		}

		Array<T>& operator=(const Array<T>& rhs) {
			if (this == &rhs) {
				return *this;
			}

			for (size_t rmvIdx = 0; rmvIdx < length_; ++rmvIdx) {
				buf_[rmvIdx].~T();
			}

			std::free(buf_);
			buf_ = nullptr;

			capacity_ = rhs.capacity_;
			length_ = rhs.length_;

			if (capacity_ > 0) {
				buf_ = static_cast<T*>(std::malloc(capacity_ * sizeof(T)));
				assert(buf_ != nullptr);

				for (size_t cpyIdx = 0; cpyIdx < length_; ++cpyIdx) {
					new (buf_ + cpyIdx) T(rhs.buf_[cpyIdx]);
				}
			}

			return *this;
		}

		Array<T>& operator=(Array<T>&& rhs) noexcept {
			if (this == &rhs) {
				return *this;
			}

			for (size_t rmvIdx{ 0 }; rmvIdx < length_; ++rmvIdx) {
				buf_[rmvIdx].~T();
			}

			std::free(buf_);


			buf_ = rhs.buf_;
			length_ = rhs.length_;
			capacity_ = rhs.capacity_;

			rhs.buf_ = nullptr;
			rhs.length_ = 0;
			rhs.capacity_ = 0;

			return *this;
		}

		void resize() {
			size_t newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;

			T* tempBuf = static_cast<T*>(malloc(newCapacity * sizeof(T)));
			assert(tempBuf != nullptr);

			for (size_t cpyIdx{ 0 }; cpyIdx < length_; cpyIdx++) {
				new (tempBuf + cpyIdx) T(std::move(buf_[cpyIdx]));
				buf_[cpyIdx].~T();
			}

			std::free(buf_);
			buf_ = tempBuf;
			capacity_ = newCapacity;
			tempBuf = nullptr;
		}

		// makes new array of doubled capacity with a blank for further insertion of new item in insert
		void resizeWithGap(size_t gapIdx) {
			size_t newCapacity = capacity_ == 0 ? 1 : capacity_ * 2;

			T* tempBuf = static_cast<T*>(malloc(newCapacity * sizeof(T)));
			assert(tempBuf != nullptr);

			for (size_t cpyIdx{ 0 }; cpyIdx < length_; cpyIdx++) {
				if (cpyIdx < gapIdx) {
					new (tempBuf + cpyIdx) T(std::move(buf_[cpyIdx]));
				}

				else {
					new (tempBuf + cpyIdx + 1) T(std::move(buf_[cpyIdx]));
				}

				buf_[cpyIdx].~T();
			}



			std::free(buf_);
			buf_ = tempBuf;
			capacity_ = newCapacity;
			tempBuf = nullptr;
		}

		// Returning index of insertion
		int insert(const T& val) {

			if (length_ >= capacity_) {
				resize();
			}

			new (buf_ + length_) T(val);
			return static_cast<int>(length_++);
		}

		int insert(T&& val) {
			if (length_ >= capacity_) {
				resize();
			}
			new (buf_ + length_) T(std::move(val)); // перемещаем объект в буфер
			return static_cast<int>(length_++);;
		}


		int insert(int index, const T& value) {
			/*
			* index MUST be in [0; length]
			* capacity is technical-only value, we can't allow use free memory as index
			*/

			assert(index >= 0);
			size_t idx = static_cast<size_t>(index);
			assert(idx <= length_);

			if (idx == length_) {
				return insert(value);
			}

			if (length_ >= capacity_) {
				// i included shift from idx to left in this function,so no need for double
				resizeWithGap(idx);
			}

			else {
				// length_ < capacity => there is already some space in the end and we can shift without reallocation
				// copy with a gap
				// first - create objects at buf[len]
				new (buf_ + length_) T(std::move(buf_[length_ - 1]));

				// all other objects are initialized and their prototypes will be overriden by left value, so I can use copy constructor
				for (size_t shiftToIdx{ length_ - 1 }; shiftToIdx > idx; shiftToIdx--) {
					buf_[shiftToIdx] = std::move(buf_[shiftToIdx - 1]);
				}
			}

			buf_[idx] = value;
			length_++;

			return index;
		}

		void remove(int index) {
			assert(index >= 0);
			size_t idx = static_cast<size_t>(index);
			assert(idx < length_);

			for (size_t shiftIdx{ idx }; shiftIdx < length_ - 1; shiftIdx++) {
				buf_[shiftIdx] = std::move(buf_[shiftIdx + 1]);
			}

			buf_[length_ - 1].~T();
			length_--;
		}

		T& operator[](int index) {
			assert(index >= 0);
			size_t idx = static_cast<size_t>(index);
			assert(idx < length_);

			return buf_[idx];
		}

		const T& operator[](int index) const {
			assert(index >= 0);
			size_t idx = static_cast<size_t>(index);
			assert(idx < length_);

			return buf_[idx];
		}

		int size() const {
			return static_cast<int> (length_);
		}

		Iterator iterator() { return Iterator(this, 0, false); }
		ConstIterator iterator() const { return ConstIterator(this, 0, false); }

		Iterator reverseIterator() {
			if (length_ == 0)
				return Iterator(this, 0, true);
			return Iterator(this, length_ - 1, true);
		}

		ConstIterator reverseIterator() const {
			if (length_ == 0)
				return ConstIterator(this, 0, true);
			return ConstIterator(this, length_ - 1, true);
		}

		// Для немодифицируемого массива
		Iterator begin() { return Iterator(this, 0); }
		Iterator end() { return Iterator(this, length_); }

		// Для константного массива
		ConstIterator cbegin() const { return ConstIterator(this, 0); }
		ConstIterator cend()   const { return ConstIterator(this, length_); }



	private:
		/* INT
		* capacity_ >= 0 && length_ >= 0;
		* capacity_ >= length_
		* if (length_ > 0) buf_ != 0
		*/

		T* buf_;
		size_t length_;
		size_t capacity_;

	};


	template<typename T>
	std::ostream& operator<<(std::ostream& os, const Array<T>& arr) {
		os << "[";

		for (size_t i = 0; i < arr.size(); ++i) {
			os << arr[i];
			if (i + 1 < arr.size())
				os << ", ";
		}

		os << "]";
		return os;
	}
}