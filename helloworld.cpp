#include <cstring>
#include <iostream>
/*
String[0]==begin
String[1...]内容
String[...]==end
*/
class String {
private://私有变量
	char* ptrString;
	size_t trueSize;
	size_t _length;  // 两倍扩容
public://公有类——迭代器
	class iterator {
	protected:
		char* ptr;
	public:
		bool operator!=(iterator iter) { return this->ptr != iter.ptr; }
		bool operator==(iterator iter) { return this->ptr == iter.ptr; }
		bool operator<=(iterator iter) { return this->ptr <= iter.ptr; }
		bool operator>=(iterator iter) { return this->ptr >= iter.ptr; }
		bool operator<(iterator iter) { return this->ptr < iter.ptr; }
		bool operator>(iterator iter) { return this->ptr > iter.ptr; }
		long long operator-(iterator step) { return ptr - step.ptr; }
		char* operator+(size_t step) { return ptr + step; }
		char* operator-(size_t step) { return ptr - step; }
		void operator+=(size_t step) { ptr += step; }
		void operator-=(size_t step) { ptr -= step; }
		char& operator*() { return *ptr; }
		char* operator&() { return ptr; }
		void operator++() { ptr++; }
		void operator--() { ptr--; }
		iterator(char* begin = nullptr) :ptr(begin) {}
	};//简化为双向迭代器
	class reverse_iterator {
	protected:
		char* ptr;
	public:
		reverse_iterator(char* rbegin) :ptr(rbegin) {}
		bool operator!=(reverse_iterator iter) { return this->ptr != iter.ptr; }
		bool operator==(reverse_iterator iter) { return this->ptr == iter.ptr; }
		bool operator>=(reverse_iterator iter) { return this->ptr <= iter.ptr; }
		bool operator<=(reverse_iterator iter) { return this->ptr >= iter.ptr; }
		bool operator>(reverse_iterator iter) { return this->ptr < iter.ptr; }
		bool operator<(reverse_iterator iter) { return this->ptr > iter.ptr; }
		long long operator-(iterator step) { return ptr - step.ptr; }
		char* operator-(size_t step) { return ptr + step; }
		char* operator+(size_t step) { return ptr - step; }
		void operator-=(size_t step) { ptr += step; }
		void operator+=(size_t step) { ptr -= step; }
		char& operator*() { return *ptr; }
		char* operator&() { return ptr; }
		void operator--() { ptr++; }
		void operator++() { ptr--; }
	};//反向迭代器
private://私有成员函数
	iterator Partition(iterator begin, iterator end) {
		iterator div = begin;
		do { ++div; } while (div != end && *div < *begin);
		iterator pile = div;
		for (--div; pile < end; ++pile) {
			if (*pile <= *begin) {
				++div;
				std::swap(*pile, *div);
			}
		}
		std::swap(*begin, *div);
		return div;
	}
	void NullptrCheck(iterator& begin, iterator& end) {
		if (begin == nullptr) begin = this->begin();
		if (end == nullptr) end = this->end();
	}
public://赋值系列
	String()
		:ptrString(nullptr), trueSize(0), _length(0) {
	};
	String(const char* input)
		: _length(strlen(input)) {
		ptrString = new char[_length * 2 + 2];
		trueSize = _length * 2;
		ptrString[0] = -1;
		strcpy_s(ptrString + 1, _length + 1, input);
	}
	String(String& input)
		: _length(strlen(input.ptrString)) {
		ptrString = new char[_length * 2 + 2];
		trueSize = _length * 2;
		strcpy_s(ptrString, _length + 1, input.ptrString);
	}
	void assign(const char* input) {
		delete[] ptrString;
		_length = strlen(input);
		ptrString = new char[_length * 2 + 2];
		trueSize = _length * 2;
		strcpy_s(ptrString, _length + 1, input);
	}
	void operator=(const char* input) { assign(input); }
	void assign(String& input) {
		delete[] ptrString;
		_length = strlen(input.c_str());
		ptrString = new char[_length * 2 + 2];
		trueSize = _length * 2;
		strcpy_s(ptrString, _length + 1, input.c_str());
	}
	void operator=(String& input) { assign(input); }
	~String() { if (ptrString != nullptr) delete[] ptrString; }
public:
	// 长度处理系列
	size_t size() { return _length; }
	size_t length() { return _length; }
	size_t capacity() { return trueSize; }
	bool empty() { return _length == 0 ? true : false; }
	void clear() {//清空字符串
		delete[] ptrString;
		ptrString = new char[2] {'\0'};
		_length = 0;
		trueSize = 2;
	}
	// 字符串访问下标-带边界检查
	char& at(size_t index) const {
		index++;
		if (index >= _length || index < 0) throw("OverFlow");  // 下标越界
		return ptrString[index];
	}
	char operator[](size_t index) { return this->at(index); }
	char& front() const{ return ptrString[1]; }
	char& back() const{ return ptrString[_length]; }
	//迭代器操作
	const char* cbegin() const { return ptrString + 1; }
	iterator begin()  const { return iterator(ptrString + 1); }
	const char* cend() const { return ptrString + _length + 1; }
	iterator end()  const { return iterator(ptrString + _length + 1); }
	const char* crbegin() const { return ptrString + _length; }
	reverse_iterator rbegin() const { return reverse_iterator(ptrString + _length); }
	const char* crend() const { return ptrString; }
	reverse_iterator rend() const { return reverse_iterator(ptrString); }
	//输出
	friend std::ostream& operator<<(std::ostream& cout, String& s) {
		std::cout << s.ptrString + 1;
		return cout;
	}// 输出
	//向后添加
	String& append(const char* str, size_t num = 1);
	String& append(const char ch, size_t num = 1) { const char str[2]{ ch,'\0' }; return this->append(str, num); }
	String& append(String& str, size_t num = 1) { return this->append(str.c_str(), num); }
	String& append(String& str, size_t index, size_t len);
	String& append(iterator start, iterator end);
	void operator+=(const char* s) { this->append(s); }
	String operator+(const char* s) {
		String strTmp = this->c_str();
		strTmp.append(s);
		return strTmp;
	}
	//尾部处理
	void push_back(const char ch) { const char str[2]{ ch,'\0' }; this->append(str); }//加入尾部字符
	void pop_back() { ptrString[_length--] = '\0'; }//删除尾部字符
	//擦除操作
	void erase(iterator first = nullptr, iterator last = nullptr) {
		NullptrCheck(first, last);//从begin删除到end前一个字符
		strcpy_s(&first, _length, &last);
		_length -= &last - &first;
	}
	void erase(char* ptr) {
		strcpy_s(ptr, strlen(ptr + 1) + 1, ptr + 1);
		_length--;
	}
	//c风格字符串
	const char* c_str() const { return ptrString + 1; }
	const char* data() const { return ptrString + 1; }
	//查找 找到返回迭代器，否则返回-1 时间复杂度O(n)
	long long find(char ch) {
		for (size_t circle = 1; circle < _length; circle++) {
			if (ptrString[circle] == ch) return circle - 1;
		}
		return -1;
	}
	iterator find(char ch, iterator iter) {
		for (; iter != this->end(); ++iter) {
			if (*iter == ch) return iter;
		}
		return nullptr;
	}
	long long find(const char* ch) {
		for (size_t circle = 1; circle < _length; circle++) {
			if (compare(&ptrString[circle], ch, end())) {
				return circle - 1;
			}
		}
		return -1;
	}
	iterator find(const char* ch, iterator iter) {
		for (; iter < this->end(); ++iter) {
			if (compare(&iter, ch, end())) {
				return &iter;
			}
		}
		return nullptr;
	}
	//从尾查找 找到返回迭代器，否则返回-1 时间复杂度O(n)
	long long rfind(char ch) {
		for (size_t circle = _length - 1; circle != 0; circle--) {
			if (ptrString[circle] == ch) return circle - 1;
		}
		return -1;
	}
	reverse_iterator rfind(char ch, reverse_iterator iter)  {
		for (; iter != this->rend(); ++iter) {
			if (*iter == ch) return iter;
		}
		return nullptr;
	}
	long long rfind(const char* ch, size_t rbegin = 0) {
		for (size_t circle = _length - 1 - rbegin; circle != 0; circle--) {
			if (compare(ptrString+circle, ch, this->end())) {
				return circle - 1;
			}
		}
		return -1;
	}
	reverse_iterator rfind(const char* ch, reverse_iterator iter) {
		for (; iter != this->rend(); ++iter) {
			if (compare(&iter, ch, this->end())) {
				return iter;
			}
		}
		return nullptr;
	}
	//字符串分割取字串
	String substr(size_t index, size_t length) {
		index++;//0->1
		auto sSize = length + 1;
		char* ret = new char[sSize];
		strncpy_s(ret, sSize, ptrString + index, length);
		String ans(ret);
		delete[] ret;
		return ans;
	}
	String substr(iterator begin, iterator end) {
		auto sSize = &end - &begin + 1;
		char* ret = new char[sSize];
		strncpy_s(ret, sSize, &begin, sSize - 1);
		String ans(ret);
		delete[] ret;
		return ans;
	}
	//index为起点插入字符
	void insert(size_t index, const char* s);
	//字串比较，如果以index为起点，包含字串substr返回true，否则返回false
	bool compare(iterator index, const char* substr, iterator end) {
		for (; *substr; ++substr, ++index) {
			if (*substr != *index) return false;
		}
		return &end - &index >= 0 ? true : false;
	}
	//替换
	void replace(iterator first, iterator last, const char* substr, const char* repalce);
	//反转
	void reverse(iterator first = nullptr, iterator last = nullptr) {
		NullptrCheck(first, last);
		--last;
		while (first < last) {
			std::swap(*first, *last);
			++first;
			--last;
		}
	}
	//排序/快排
	void sort(iterator first = nullptr, iterator last = nullptr) {
		NullptrCheck(first, last);
		if (first >= last) return;//end sort
		iterator div = Partition(first, last);
		sort(first, div);
		sort(div + 1, last);
	}
	//仿std::unique编写
	char* unique(iterator first = nullptr, iterator last = nullptr) {
		NullptrCheck(first, last);
		if (first == last) return &last;
		iterator low = first;
		for (++first; first != last; ++first) {
			if (*low != *first) {
				++low;
				*low = *first;
			}
		}
		++low;
		return &low;
	}
	//我保留了最初的版本
	//char* unique(iterator first = nullptr, iterator last = nullptr) {
	//	NullptrCheck(first, last);
	//	if (first == last) return &last;
	//	iterator high = first;
	//	while (true) {
	//		do { ++high; } while (*high == *first);
	//		++first;
	//		if (&high == &last) return &first;//伪去重，返回重复开始的地址
	//		std::swap(*high, *first);
	//	}
	//}
};
String& String::append(const char* str, size_t num) {
	size_t strSize = strlen(str);
	size_t sSize = strSize * num;
	size_t finalSize = sSize + _length;
	if (finalSize >= trueSize) {
		char* ptrTmp = ptrString;
		ptrString = new char[finalSize * 2 + 2];
		trueSize = finalSize * 2;
		strcpy_s(ptrString, _length + 2, ptrTmp);
		for (size_t c = 0; c < num; ++c) {
			strcpy_s(ptrString + _length + 1, strSize + 1, str);
			_length += strSize;
		}
		delete[] ptrTmp;
	} else {
		for (size_t c = 0; c < num; ++c) {
			strcpy_s(ptrString + _length + 1, strSize + 1, str);
			_length += strSize;
		}
	}
	return *this;
}
String& String::append(iterator first, iterator last) {
	size_t  strSize = &last - &first;
	size_t finalSize = strSize + _length;
	if (finalSize >= trueSize) {
		char* ptrTmp = ptrString;
		ptrString = new char[finalSize * 2 + 2];
		trueSize = finalSize * 2;
		strcpy_s(ptrString, _length + 2, ptrTmp);
		iterator iter = this->end();
		for (; first != last; ++iter, ++first) { *iter = *first; }
		*iter = '\0';
		delete[] ptrTmp;
	} else {
		iterator iter = this->end();
		for (; first != last; ++iter, ++first) { *iter = *first; }
		*iter = '\0';
	}
	_length += strSize;
	return *this;
}
String& String::append(String& str, size_t index, size_t len) {
	iterator first = str.begin() + index;
	iterator last = first + len;
	return this->append(first, last);
}
void String::insert(size_t index, const char* s) {
	index++;
	auto len = strlen(s);
	auto finalSize = len + _length;
	if (finalSize >= trueSize) {
		char* ptrTmp = ptrString;
		ptrString = new char[finalSize * 2 + 2];
		strncpy_s(ptrString, index + 1, ptrTmp, index);
		strcpy_s(ptrString + index, finalSize, s);
		strcpy_s(ptrString + index + len, strlen(ptrTmp + index) + 1, ptrTmp + index);
		trueSize = finalSize * 2;
		_length += len;
		delete[] ptrTmp;
	} else {
		auto rightSize = strlen(ptrString + index) + 1;
		char* ptrTmp = new char[rightSize];
		strcpy_s(ptrTmp, rightSize, ptrString + index);
		strcpy_s(ptrString + index, len + 1, s);
		strcpy_s(ptrString + index + len, rightSize, ptrTmp);
		_length += len;
		delete[] ptrTmp;
	}
}
void String::replace(iterator first, iterator last, const char* substr, const char* repalce) {
	auto sSize = strlen(substr);
	auto rSize = strlen(repalce);
	while (first != last) {
		if (compare(&first, substr, &last) == true) {
			auto beginLen = strlen(&first);
			char* strTmp = new char[beginLen - sSize + 1];
			strcpy_s(strTmp, beginLen - sSize + 1, &first + sSize);
			this->erase(&first, this->end());
			this->append(repalce);
			this->append(strTmp);
			first += rSize;
			last += rSize - sSize;
			delete[] strTmp;
		} else ++first;
	}
}
signed main() {
	String s = "end";
	s.append(s, 0, 3);
	s.at(0) = '5';
	std::cout << s.c_str();
	return 0;
}