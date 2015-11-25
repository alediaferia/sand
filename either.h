#ifndef EITHER_H
#define EITHER_H

template <class T1, class T2> 
class Either
{
public:
    Either(const T1 &t1) : 
      _isLeft(true),
      _left(t1) {}

    Either(T1 &&t1) :
      _isLeft(true)
    {
        _left = std::move(t1);
    }

    Either(const T2 &t2) :
      _isLeft(false),
      _right(t2) {}

     Either(T2 &&t2) :
       _isLeft(false) {
        _right = std::move(t2);
     }

     Either(const Either<T1,T2> &cp) :
       _isLeft(cp._isLeft) {
       if (_isLeft) {
           _left = cp._left;
       } else {
           _right = cp._right;
       }
     }

     ~Either() {}

     bool isLeft() const { return _isLeft; }
     bool isRight() const { return !_isLeft; }

     T1& left() const { return _left; }
     T2& right() const { return _right; }

private:
    bool _isLeft;
    union
    {
        T1 _left;
        T2 _right;    
    };

public:
    static Either<T1, T2> Left(T1 x) {
        return Either<T1, T2>(x);
    }
    static Either<T1, T2> Right(T2 x) {
        return Either<T1, T2>(x);
    }
};

#endif // EITHER_H

