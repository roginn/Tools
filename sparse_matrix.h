#ifndef SPARSEMATRIX_H_
#define SPARSEMATRIX_H_

#include <iostream>
#include <map>
#include <vector>
#include <functional>

typedef unsigned int uint;

template <typename T>
class SparseMatrix
{
public:
  typedef std::map<uint, T> Row;
	typedef typename Row::iterator RowIter;
	typedef typename Row::const_iterator CRowIter;

	SparseMatrix() {}
	SparseMatrix(const SparseMatrix<T>& other){
		other.iterate(
			[&](uint row, CRowIter& r_it){
				this->set(row, r_it->first, r_it->second);
			}
		);		
	}
	virtual ~SparseMatrix() {}

	uint numRows() const{ return matrix_.size(); }

	T at(uint row, uint col) const {
		if(row >= matrix_.size())
			return 0;
		CRowIter it = matrix_.at(row).find(col);
		return (it == matrix_.at(row).end()) ? 0 : it->second;
	}

	bool set(uint row, uint col, const T& elem){
		if(elem == 0) 
			return false;

		if(matrix_.size() <= row)
			matrix_.resize(row+1);

		std::pair<uint, T> p = std::make_pair(col, elem);
		return matrix_.at(row).insert(p).second;
	}

	const Row& getRow(uint index) const {
		return matrix_.at(index);
	}

	void iterate(std::function<void (uint row, CRowIter& r_it)> iter) const {
		for(uint i = 0; i < matrix_.size(); i++){
			const auto& row = matrix_.at(i);
			for(auto r_it = row.begin(); r_it != row.end(); r_it++){
				iter(i, r_it);
			}
		}		
	}

	SparseMatrix<T> transpose() const {
		SparseMatrix<T> result;
		iterate(
			[&](uint row, CRowIter& r_it){
				result.set(r_it->first, row, r_it->second);
			}
		);
		return result;
	}

	SparseMatrix<T> operator*(const SparseMatrix<T>& other) const {
		SparseMatrix<T> result;
		SparseMatrix<T> trans = other.transpose();
		for (uint i = 0; i < matrix_.size(); ++i)
		{
			for (uint j = 0; j < trans.numRows(); ++j)
			{
				// this takes O(n log(m)), let's make sure 
				// that n is the smaller one
				bool invert = trans.getRow(j).size() < matrix_.at(i).size();
				auto& small = invert ? trans.getRow(j) : matrix_.at(i);
				auto& big = invert ? *this : trans;
				uint index = invert ? i : j;

				T sum = 0;
				CRowIter end = small.end();
				for(CRowIter it = small.begin(); it != end; it++){
					T at_other = big.at(index, it->first);
					if(at_other != 0)
						sum += at_other * it->second;
				}
				result.set(i, j, sum);
			}
		}
		
		return result;
	}

	template <typename U>
	friend std::ostream& operator<<(std::ostream&, const SparseMatrix<U>&);

private:
	std::vector<std::map<uint, T> > matrix_;
};

template<typename T>
std::ostream& operator<<(std::ostream& os, const SparseMatrix<T>& sm) {
	typedef typename SparseMatrix<T>::CRowIter CRowIter;
	sm.iterate([&](uint row, CRowIter& r_it){
		bool row_begin = (r_it == sm.getRow(row).begin());
		if(row_begin)
			os << "\n" << row << ":";
		else
			os << "->";				

		os << "(" << r_it->first << ", " << r_it->second << ")";
	});
	os << "\n";
	return os;
}

#endif // SPARSEMATRIX_H_
