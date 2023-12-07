#pragma once

//
// Created by franciscolopez on 02/08/2023.
//

#ifndef ENSAYO_SERIALIZABLE_H
#define ENSAYO_SERIALIZABLE_H

#include <cstdint>
#include <iostream>
#include <vector>

/**
 * @brief Interface to make serializable classes.
 *
 * This class provides an interface for making classes serializable. To make a class
 * serializable, it must inherit from this class and implement the `operator const
 * std::vector<uint8_t>()` method. This method should return a vector containing
 * the serialized data for the object.
 */
class Serializable {
public:
    /**
     * @brief Default constructor.
     */
    Serializable() = default;

    /**
     * @brief Constructor that takes a serialized data vector.
     *
     * @param serializedData The serialized data vector.
     */
    explicit Serializable(const std::vector<uint8_t> &serializedData);

    /**
     * @brief Returns the serialized data for the object.
     *
     * @return The serialized data vector.
     */
    virtual explicit operator const std::vector<uint8_t>() const;

    /**
     * @brief Destructor.
     */
    virtual ~Serializable() = default;

    /**
     * @brief Returns the size of the serialized data.
     *
     * @return The size of the serialized data in bytes.
     */
    int size();

    bool empty() const {
        return serializedData.empty();
    }

protected:
    /**
     * @brief The serialized data vector.
     */
    std::vector<uint8_t> serializedData;

    /**
     * @brief Sets the serialized data vector.
     *
     * @param serializedData The serialized data vector.
     */
    void setVector(std::vector<uint8_t> serializedData);

    /**
     * @brief Friend operator to print the object to an output stream.
     *
     * @param os The output stream.
     * @param serializable The object to print.
     *
     * @return The output stream.
     */
    friend std::ostream &operator<<(std::ostream &os, const Serializable &serializable);
};

#endif // ENSAYO_SERIALIZABLE_H