#pragma once
#include <algorithm>
#include <iterator>


template <typename _Ty, class _Alloc = std::allocator <_Ty>>
class CircularBuffer {
private: 
	class circ_iterator;
	class const_circ_iterator;

public:

	using value_type = _Ty;
	using allocator_type = _Alloc;
	using alloc_traits = std::allocator_traits <allocator_type>;

	using pointer = typename alloc_traits::pointer;
	using const_pointer = typename alloc_traits::const_pointer;
	using size_type = typename alloc_traits::size_type;
	using difference_type = typename alloc_traits::difference_type;

	using reference = value_type&;
	using const_reference = value_type const&;
	using iterator = circ_iterator;
	using const_iterator = const_circ_iterator;


	CircularBuffer(): CircularBuffer(_Alloc()) {};

	CircularBuffer(CircularBuffer&& _Right, const allocator_type& _Al) :
		mSize_(_Right.mSize_),
		mCapacity_(_Right.mCapacity_),
		mBegin_(_Right.mBegin_),
		mEnd_(_Right.mEnd_),
		mpArr_(_Right.mpArr_),
		mAl_(_Al)
	{
		_Right.mpArr_ = nullptr;
	}

	CircularBuffer(CircularBuffer&& _Right) : CircularBuffer(_Right, std::move(_Right.mAl_)) {};

	CircularBuffer(const size_type& _Count, const allocator_type& _Al = allocator_type()) :
		mCapacity_(_Count),
		mSize_(_Count * 2 + 8),
		mBegin_(0),
		mEnd_(_Count + 1),
		mAl_(_Al),
		mpArr_(alloc_traits::allocate(mAl_, mSize_))
	{
		if (std::is_trivially_constructible <value_type>::value) {
			for (size_type i = 1; i <= _Count; i++)
				alloc_traits::construct(mAl_, mpArr_ + i);
		}
	}

	CircularBuffer(const size_type _Count, const_reference _Val, const allocator_type& _Al = allocator_type()) :
		mCapacity_(_Count),
		mSize_(_Count * 2 + 8),
		mBegin_(0),
		mEnd_(0),
		mAl_(_Al),
		mpArr_(alloc_traits::allocate(mAl_, mSize_))
	{
		if (std::is_constructible <value_type>::value) {
			for (size_type i = 0; i < _Count; i++)
				alloc_traits::construct(mAl_, mpArr_ + i, _Val);
		}
	}

	CircularBuffer(const std::initializer_list <value_type>& _Ilist, const allocator_type& _Al = allocator_type()) :
		CircularBuffer(_Ilist.begin(), _Ilist.end(), _Al) {};

	CircularBuffer(const CircularBuffer& other) : CircularBuffer(other, other.mAl_) {};

	CircularBuffer(const CircularBuffer& other, const allocator_type& _Al) :
		mSize_(other.mSize_),
		mCapacity_(other.mCapacity_),
		mBegin_(other.mBegin_),
		mEnd_(other.mEnd_),
		mAl_(_Al),
		mpArr_(mAl_.allocate(mSize_))
	{
		if (std::is_copy_constructible <value_type>::value) {
			if (std::is_trivially_constructible <value_type>::value) {
				std::memcpy(mpArr_, other.mpArr_, mSize_);
			}
			else {
				for (size_type i = 0; i < other.size(); i++)
					alloc_traits::construct(mAl_, mpArr_ + i, other[i]);
			}
		}
	}

	explicit CircularBuffer(const allocator_type& _Al) : CircularBuffer(0, _Al) {};

	template <class _Iter> 
	CircularBuffer(_Iter _First, _Iter _Last, const allocator_type& _Al = allocator_type()) :
		mCapacity_(_Last - _First),
		mSize_(mCapacity_ * 2 + 8),
		mBegin_(0),
		mEnd_(mCapacity_ + 1),
		mAl_(_Al),
		mpArr_(alloc_traits::allocate(mAl_, mSize_))
	{
		if (std::is_move_assignable <_Ty>::value) {
			size_type i = 1;
			while (_First != _Last)
				alloc_traits::construct(mAl_, mpArr_ + (i++),*(_First++));
		}
	}

	CircularBuffer& operator=(const CircularBuffer& _Right) {
		if (this != &_Right)
			swap(*this, CircularBuffer(_Right));
		return *this;
	}

	CircularBuffer& operator=(CircularBuffer&& _Right) noexcept {
		if (this != &_Right)
			swap(*this, _Right);
		return *this;
	}

	iterator begin() {
		return iterator(mpArr_, mCapacity_, mSize_, 0, (mBegin_ + 1) % mSize_);
	}

	iterator end() {
		return iterator(mpArr_, mCapacity_, mSize_, mCapacity_ , (mBegin_ + 1) % mSize_);
	}

	const_iterator cbegin() const {
		return const_iterator(mpArr_, mSize_, (mBegin_ + 1) % mSize_, mBegin_);
	}

	const_iterator cend() const {
		return const_iterator(mpArr_, mSize_, mEnd_, mBegin_);
	}

	void push_back(const_reference _Val) {
		if (mCapacity_ + 1 == mSize_)
			this->resize(mSize_ + 8);

		++mCapacity_;

		alloc_traits::construct(mAl_, mpArr_ + mEnd_, _Val);

		if (mBegin_ == mEnd_) {
			if (mBegin_)
				--mBegin_;
			else
				mBegin_ = mSize_ - 1;
		}
		mEnd_ = (mEnd_ + 1) % mSize_;

	}

	void pop_back() noexcept{
		if (!mCapacity_)
			return;
		if (mEnd_)
			mEnd_--;
		else
			mEnd_ = mSize_ - 1;
		mCapacity_--;

		if (std::is_destructible <value_type>::value)
			alloc_traits::destroy(mAl_, mpArr_ + mEnd_);
	}

	void push_front(const _Ty& _Val) {
		if (mCapacity_ + 1 == mSize_)
			this->resize(mSize_ + 8);
		++mCapacity_;

		alloc_traits::construct(mAl_, mpArr_ + mBegin_, _Val);
		if (mBegin_ == mEnd_) {
			mEnd_ = (mEnd_ + 1) % mSize_;
		}
		if (mBegin_)
			--mBegin_;
		else
			mBegin_ = mSize_ - 1;

	}

	void pop_front() {
		if (!mCapacity_)
			return;
		mCapacity_--;
		mBegin_ = (mBegin_ + 1) % mSize_;
		if (std::is_destructible <_Ty>::value)
			mpArr_[mBegin_]._Ty::~_Ty();
	}

	reference operator[](size_type _Pos) {
		return *(mpArr_ + (mBegin_ + _Pos + 1) % mSize_);
	}


	const_reference operator[](size_type _Pos) const {
		return *(mpArr_ + (mBegin_ + _Pos + 1) % mSize_);
	}

	size_type size() const {
		return mCapacity_;
	}

	size_type capacity() const {
		return mSize_;
	}

	bool empty() const {
		return std::abs(this->cbegin() - this->cend()) == 0;
	}

	allocator_type get_allocator() const {
		return mAl_;
	}

	reference front() {
		return (*this)[0];
	}

	reference back() {
		return (*this)[mCapacity_ - 1];
	}

	void reserve(size_type _NewSize) {
		if (_NewSize > mSize_)
			this->resize(_NewSize);
	}

	void shrink_to_fit() const {
		this->resize(mCapacity_ + 8);
	}

	bool operator==(const CircularBuffer& _Right) {
		if (_Right.empty() && this->empty())
			return true;
		if (this->capacity != _Right.capacity)
			return false;

		const_iterator lIt= this->cbegin();
		const_iterator rIt= _Right.cbegin();

		for (; lIt != this->cend(); ++lIt, ++rIt)
			if (*lIt != *rIt)
				return false;

		return true;
	}

	bool operator!=(const CircularBuffer& _Right) {
		return !(*this == _Right);
	}

	friend void swap(CircularBuffer& _Lhs, CircularBuffer& _Rhs) {
		std::swap(_Lhs.mCapacity_, _Rhs.mCapacity_);
		std::swap(_Lhs.mSize_, _Rhs.mSize_);
		std::swap(_Lhs.mBegin_, _Rhs.mBegin_);
		std::swap(_Lhs.mEnd_, _Rhs.mEnd_);
		std::swap(_Lhs.mpArr_, _Rhs.mpArr_);
		if (_Lhs.mAl_ != _Rhs.mAl_) {
			std::swap(_Lhs.mAl_, _Rhs.mAl_);
		}
	}

	~CircularBuffer() {
		if (!std::is_trivially_destructible <_Ty>::value) {
			for (size_type i = 0; i < mCapacity_; ++i)
				alloc_traits::destroy(mAl_, mpArr_ + i);
		}

		alloc_traits::deallocate(mAl_, mpArr_, mSize_);
	}

private:

	void resize(size_type _NewSize) {
		pointer NewArr = alloc_traits::allocate(mAl_, _NewSize);
		size_type i = 0;
		for (; i < this->size(); ++i)
			alloc_traits::construct(mAl_, NewArr + i, std::move((*this)[i]));

		mBegin_ = _NewSize - 1;
		mEnd_ = i;
		alloc_traits::deallocate(mAl_, mpArr_, mSize_);
		mSize_ = _NewSize;
		mpArr_ = NewArr;
	}

	size_type mCapacity_;
	size_type mSize_;
	difference_type mBegin_;
	difference_type mEnd_;
	pointer mpArr_;
	allocator_type mAl_;
};

template <typename _Ty, class _Alloc>
class CircularBuffer<_Ty, _Alloc>::circ_iterator {
private:
	pointer mpArr_;
	size_type mSize_;
	size_type mCapacity_;
	size_type mOffset_;
	size_type mBegin_;
public:
	using value_type = CircularBuffer::value_type;
	using element_type = CircularBuffer::value_type;
	using pointer = CircularBuffer::pointer;
	using reference = CircularBuffer::reference;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = CircularBuffer::difference_type;

	circ_iterator(pointer _Data, size_type _Cap, size_type _Size, size_type _Ofst, size_type _Begin) :
		mpArr_(_Data),
		mSize_(_Size),
		mCapacity_(_Cap),
		mOffset_(_Ofst),
		mBegin_(_Begin)
	{};
	circ_iterator(const circ_iterator& other) :
		mpArr_(other.mpArr_),
		mSize_(other.mSize_),
		mCapacity_(other.mCapacity_),
		mOffset_(other.mOffset_),
		mBegin_(other.mBegin_)
	{};


	pointer operator->() const {
		return mpArr_ + (mBegin_ + (mOffset_ % mCapacity_)) % mSize_;
	}

	reference operator*() const {
		return *(mpArr_ + (mBegin_ + (mOffset_ % mCapacity_)) % mSize_);
	}

	circ_iterator& operator++() {
		++mOffset_;
		return *this;
	}
	circ_iterator operator--() {
		*this -= 1;
		return *this;
	}

	circ_iterator operator++(int) {
		circ_iterator it(*this);
		++mOffset_;
		return it;
	}

	circ_iterator operator--(int) {
		circ_iterator it(*this);
		*this -= 1;
		return it;
	}

	circ_iterator& operator+=(difference_type _Off) {
		mOffset_ += _Off;
		return *this;
	}

	circ_iterator& operator-=(difference_type _Off) {
		if (mOffset_ < _Off)
			mOffset_ = mCapacity_ - (_Off % mCapacity_ - mOffset_);
		else
			mOffset_ -= _Off;
		return *this;
	}

	circ_iterator operator-(difference_type _Off) const {
		return circ_iterator(*this) -= _Off;
	}

	difference_type operator-(const circ_iterator _Right) const {
		return (this->mOffset_ % (mCapacity_ + 1)) - (_Right.mOffset_ % (mCapacity_ + 1));
	}

	circ_iterator operator+(difference_type _Off) const {
		return circ_iterator(*this) += _Off;
	}


	friend auto operator<=>(const circ_iterator& _Left, const circ_iterator& _Right) = default;
};

template <typename _Ty, class _Alloc>
class CircularBuffer<_Ty, _Alloc>::const_circ_iterator {
private:
	pointer mpArr_;
	size_type mSize_;
	size_type mCapacity_;
	size_type mOffset_;
	size_type mBegin_;
public:
	using value_type = CircularBuffer::value_type;
	using element_type = CircularBuffer::value_type;
	using pointer = CircularBuffer::pointer;
	using reference = CircularBuffer::reference;
	using iterator_category = std::random_access_iterator_tag;
	using difference_type = CircularBuffer::difference_type;

	const_circ_iterator(pointer _Data, size_type _Cap, size_type _Size, size_type _Ofst, size_type _Begin) :
		mpArr_(_Data),
		mSize_(_Size),
		mCapacity_(_Cap),
		mOffset_(_Ofst),
		mBegin_(_Begin)
	{};
	const_circ_iterator(const circ_iterator& other) :
		mpArr_(other.mpArr_),
		mSize_(other.mSize_),
		mCapacity_(other.mCapacity_),
		mOffset_(other.mOffset_),
		mBegin_(other.mBegin_)
	{};


	pointer operator->() const {
		return mpArr_ + (mBegin_ + (mOffset_ % mCapacity_)) % mSize_;
	}

	reference operator*() const {
		return *(mpArr_ + (mBegin_ + (mOffset_ % mCapacity_)) % mSize_);
	}


	const_circ_iterator operator-(difference_type _Off) const {
		return const_circ_iterator(*this) -= _Off;
	}

	difference_type operator-(const const_circ_iterator _Right) const {
		return (this->mOffset_ & (mCapacity_ + 1)) - (_Right.mOffset_ & (mCapacity_ + 1));
	}

	const_circ_iterator operator+(difference_type _Off) const {
		return const_circ_iterator(*this) += _Off;
	}

	friend auto operator<=>(const const_circ_iterator& _Left, const const_circ_iterator& _Right) = default;

private:
	circ_iterator& operator+=(difference_type _Off) {
		mOffset_ += _Off;
		return *this;
	}

	circ_iterator& operator-=(difference_type _Off) {
		if (mOffset_ < _Off)
			mOffset_ = mCapacity_ - (_Off % mCapacity_ - mOffset_);
		else
			mOffset_ -= _Off;
		return *this;
	}

};