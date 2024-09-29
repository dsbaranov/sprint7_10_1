#pragma once

#include <cassert>
#include <initializer_list>
#include <algorithm>
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

  // Возвращает количество элементов в массиве
  size_t GetSize() const noexcept
  {
    return size_;
  }

  // Возвращает вместимость массива
  size_t GetCapacity() const noexcept
  {
    return capacity_;
  }

  // Сообщает, пустой ли массив
  bool IsEmpty() const noexcept
  {
    return size_ == 0;
  }

  // Возвращает ссылку на элемент с индексом index
  Type &operator[](size_t index) noexcept
  {
    return items_[index];
  }

  // Возвращает константную ссылку на элемент с индексом index
  const Type &operator[](size_t index) const noexcept
  {
    return items_[index];
  }

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
  void Clear() noexcept
  {
    size_ = 0;
  }

  // Изменяет размер массива.
  // При увеличении размера новые элементы получают значение по умолчанию для типа Type
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

  // Возвращает итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  Iterator begin() noexcept
  {
    return &items_[0];
  }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  Iterator end() noexcept
  {
    return &items_[size_];
  }

  // Возвращает константный итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator begin() const noexcept
  {
    return &items_[0];
  }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator end() const noexcept
  {
    return &items_[size_];
  }

  // Возвращает константный итератор на начало массива
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator cbegin() const noexcept
  {
    return &items_[0];
  }

  // Возвращает итератор на элемент, следующий за последним
  // Для пустого массива может быть равен (или не равен) nullptr
  ConstIterator cend() const noexcept
  {
    return &items_[size_];
  }

private:
  ArrayPtr<Type> items_;
  size_t capacity_ = 0;
  size_t size_ = 0;
};