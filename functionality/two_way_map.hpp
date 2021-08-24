#ifndef TWO_WAY_MAP_HPP
#define TWO_WAY_MAP_HPP

#include <unordered_map>
#include <iostream>

template <class T, class U>
class TwoWayMap {

private:
  std::unordered_map<T, U> t_to_u;
  std::unordered_map<U, T> u_to_t;

  template <class V, class W>
  W get_or_default_helper(V key, W return_if_key_missing,
			  std::unordered_map<V, W> v_to_w);

  template <class V, class W>
  void erase_helper(V v_object, std::unordered_map<V, W>& v_to_w,
		    std::unordered_map<W, V>& w_to_v);

public:
  void put(T t_object, U u_object);
  void put(U u_object, T t_object);

  T get_or_default(U key, T return_if_key_missing);
  U get_or_default(T key, U return_if_key_missing);

  void erase(T key);
  void erase(U key);

  bool contains(T key);
  bool contains(U key);
};


template <class T, class U>
void TwoWayMap<T, U>::put(T t_object, U u_object) {
  if(contains(t_object)) {
    erase(t_object);
  }
  if(contains(u_object)) {
    erase(u_object);
  }
  
  t_to_u[t_object] = u_object;
  u_to_t[u_object] = t_object;
}

template <class T, class U>
void TwoWayMap<T, U>::put(U u_object, T t_object) {
  put(t_object, u_object);
}

template <class T, class U>
void TwoWayMap<T, U>::erase(U key) {
  erase_helper(key, u_to_t, t_to_u);
}

template <class T, class U>
void TwoWayMap<T, U>::erase(T key) {
  erase_helper(key, t_to_u, u_to_t);
}

template <class T, class U>
template <class V, class W>
void TwoWayMap<T, U>::erase_helper(V v_object,
				   std::unordered_map<V, W>& v_to_w,
				   std::unordered_map<W, V>& w_to_v) {
  if(contains(v_object)) {
    W w_object = v_to_w[v_object];
    
    v_to_w.erase(v_object);
    w_to_v.erase(w_object);
  }
}


template <class T, class U>
T TwoWayMap<T, U>::get_or_default(U key, T return_if_key_missing) {
  return get_or_default_helper<U, T>(key, return_if_key_missing, u_to_t);
}

template <class T, class U>
U TwoWayMap<T, U>::get_or_default(T key, U return_if_key_missing) {
  return get_or_default_helper<T, U>(key, return_if_key_missing, t_to_u);
}

template <class T, class U>
template <class V, class W>
W TwoWayMap<T, U>::get_or_default_helper(V key, W return_if_key_missing,
					 std::unordered_map<V, W> v_to_w) {
  W to_return;
  if(v_to_w.contains(key)) {
    to_return = v_to_w[key];

  } else {
    to_return = return_if_key_missing;
  }

  return to_return;
}

template <class T, class U>
bool TwoWayMap<T, U>::contains(T key) {
  return t_to_u.contains(key);
}

template <class T, class U>
bool TwoWayMap<T, U>::contains(U key) {
  return u_to_t.contains(key);
}

#endif
