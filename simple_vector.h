#pragma once
#include <algorithm>
#include <initializer_list>

#include "array_ptr.h"

template <typename Type>
class SimpleVector
{
public:
  using Iterator = Type *;
  using ConstIterator = const Type *;

  SimpleVector() noexcept = default;

  // Создаёт вектор из size элементов, инициализированных значением по умолчанию
  explicit SimpleVector(size_t size)
  {
    ArrayPtr<Type> new_array(size);
    for (size_t index = 0; index < size; ++index)
    {
      new_array[index] = Type();
    }
    items_.swap(new_array);
    capacity_ = size;
    size_ = size;
  }

  // Создаёт вектор из size элементов, инициализированных значением value
  SimpleVector(size_t size, const Type &value)
  {
    ArrayPtr<Type> new_array(size);
    for (size_t index = 0; index < size; ++index)
    {
      new_array[index] = value;
    }
    items_.swap(new_array);
    capacity_ = size;
    size_ = size;
  }

  // Создаёт вектор из std::initializer_list
  SimpleVector(std::initializer_list<Type> init)
  {
    size_t size = init.size();
    ArrayPtr<Type> new_array(init.size());
    size_t index = 0;
    for (auto init_it = init.begin(); init_it != init.end(); ++init_it)
    {
      new_array[index++] = *init_it;
    }
    items_.swap(new_array);
    capacity_ = size;
    size_ = size;
  }

  SimpleVector(const SimpleVector &other)
  {
    capacity_ = other.GetCapacity();
    size_ = other.GetSize();
    ArrayPtr<Type> new_items(capacity_);
    items_.swap(new_items);
    auto other_it = other.begin();
    auto this_it = begin();
    while (other_it != other.end())
    {
      *this_it++ = *other_it++;
    }
  }

  SimpleVector &operator=(const SimpleVector &rhs)
  {
    capacity_ = rhs.GetCapacity();
    size_ = rhs.GetSize();
    ArrayPtr<Type> new_items(capacity_);
    items_.swap(new_items);
    std::copy(rhs.begin(), rhs.end(), begin());
    return *this;
  }

  // Добавляет элемент в конец вектора
  // При нехватке места увеличивает вдвое вместимость вектора
  void PushBack(const Type &item)
  {
    if (size_ == capacity_)
    {
      ExpandSelfCapacity();
    }
    items_[size_++] = item;
  }

  // Вставляет значение value в позицию pos.
  // Возвращает итератор на вставленное значение
  // Если перед вставкой значения вектор был заполнен полностью,
  // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0
  // стать равной 1
  Iterator Insert(ConstIterator pos, const Type &value)
  {
    auto dst = std::distance(cbegin(), pos);
    if (size_ == capacity_)
    {
      ExpandSelfCapacity();
    }
    size_++;
    std::copy_backward(begin() + dst, end() - 1, end());
    auto it = begin() + dst;
    *it = value;
    return it;
  }

  // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
  void PopBack() noexcept
  {
    if (size_ > 0)
    {
      --size_;
    }
  }

  // Удаляет элемент вектора в указанной позиции
  Iterator Erase(ConstIterator pos)
  {
    auto distance = std::distance(cbegin(), pos);
    Iterator it = begin() + distance;
    while (it + 1 != end())
    {
      *it = *(it + 1);
      it++;
    }
    size_--;
    return (begin() + distance);
  }

  // Обменивает значение с другим вектором
  void swap(SimpleVector &other) noexcept
  {
    size_t other_size = other.GetSize();
    size_t other_capacity = other.GetCapacity();
    items_.swap(other.Get());
    other.SetSize(size_);
    other.SetCapacity(capacity_);
    size_ = other_size;
    capacity_ = other_capacity;
  }

  // Возвращает количество элементов в массиве
  size_t GetSize() const noexcept { return size_; }

  void SetSize(size_t other_size) { size_ = other_size; }

  // Возвращает вместимость массива
  size_t GetCapacity() const noexcept { return capacity_; }

  void SetCapacity(size_t other_capacity) { capacity_ = other_capacity; }

  // Сообщает, пустой ли массив
  bool IsEmpty() const noexcept { return size_ == 0; }

  // Возвращает ссылку на элемент с индексом index
  Type &operator[](size_t index) noexcept { return items_[index]; }

  // Возвращает константную ссылку на элемент с индексом index
  const Type &operator[](size_t index) const noexcept { return items_[index]; }

  // Возвращает константную ссылку на элемент с индексом index
  // Выбрасывает исключение std::out_of_range, если index >= size
  Type &At(size_t index)
  {
    if (index >= size_)
      throw std::out_of_range("out of range");
    return items_[index];
  }

  // Возвращает константную ссылку на элемент с индексом index
  // Выбрасывает исключение std::out_of_range, если index >= size
  const Type &At(size_t index) const
  {
    if (index >= size_)
      throw std::out_of_range("out of range");
    return items_[index];
  }

  // Обнуляет размер массива, не изменяя его вместимость
  void Clear() noexcept { size_ = 0; }

  // Изменяет размер массива.
  // При увеличении размера новые элементы получают значение по умолчанию для
  // типа Type
  void Resize(size_t new_size)
  {
    if (new_size <= capacity_)
    {
      if (new_size <= size_)
      {
        size_ = new_size;
      }
      else
      {
        for (size_t index = size_; index < new_size; ++index)
        {
          items_[index] = Type();
        }
        size_ = new_size;
      }
    }
    else
    {
      ArrayPtr<Type> new_items(new_size);
      for (size_t index = 0; index < new_size; ++index)
      {
        new_items[index] = Type();
      }
      for (size_t index = 0; index < size_; ++index)
      {
        new_items[index] = items_[index];
      }
      items_.swap(new_items);
      size_ = capacity_ = new_size;
    }
  }

  void Reserve(size_t new_capacity)
  {
    ArrayPtr<Type> new_items(new_capacity);
    std::copy(begin(), end(), new_items.Get());
    capacity_ = new_capacity;
    items_.swap(new_items);
  };

  ArrayPtr<Type> &Get() { return items_; }

  ArrayPtr<Type> &Get() const { return items_; }

  // Возвращает итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  Iterator begin() noexcept { return &items_[0]; }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  Iterator end() noexcept { return &items_[size_]; }

  // Возвращает константный итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator begin() const noexcept { return &items_[0]; }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator end() const noexcept { return &items_[size_]; }

  // Возвращает константный итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator cbegin() const noexcept { return &items_[0]; }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator cend() const noexcept { return &items_[size_]; }

private:
  ArrayPtr<Type> items_;
  size_t capacity_ = 0;
  size_t size_ = 0;

  void ExpandSelfCapacity()
  {
    if (capacity_ == 0)
    {
      ++capacity_;
    }
    else
    {
      capacity_ *= 2u;
    }
    ArrayPtr<Type> new_items(capacity_);
    std::copy(begin(), end(), new_items.Get());
    items_.swap(new_items);
  }
};

template <typename Type>
inline bool operator==(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs)
{
  if (lhs.GetSize() != rhs.GetSize())
    return false;
  return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs)
{
  return !(lhs == rhs);
}

template <typename Type>
inline bool operator<(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs)
{
  return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(),
                                      rhs.end());
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs)
{
  return lhs < rhs || lhs == rhs;
}

template <typename Type>
inline bool operator>(const SimpleVector<Type> &lhs,
                      const SimpleVector<Type> &rhs)
{
  return !(lhs <= rhs);
}

template <typename Type>
inline bool operator>=(const SimpleVector<Type> &lhs,
                       const SimpleVector<Type> &rhs)
{
  return !(lhs < rhs);
}