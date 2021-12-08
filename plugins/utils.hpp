#ifndef CROSSER_UTILS_HPP
#define CROSSER_UTILS_HPP

const static int width = 120;
const static int height = 120;

const static int blockSize = 3;
const static float mapSize = width * blockSize;

template<typename Base, typename T>
inline bool instanceof(const T*) {
	return std::is_base_of<Base, T>::value;
}

#endif //CROSSER_UTILS_HPP