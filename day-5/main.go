package main

import (
	"fmt"
	"log"
	"math"
	"os"
	"sort"
	"strconv"
	"strings"
)

type transform struct {
	srcStart int
	dstStart int
	_range   int
}

type seedRange struct {
	start int
	end   int
}

func main() {
	// input := make([]int, 0, 53203352)
	inputRange := []seedRange{}
	transforms := [][]transform{}
	currentTransform := []transform{}

	data, err := os.ReadFile("input.txt")
	if err != nil {
		log.Fatal("Failed to read file")
		return
	}

	lines := strings.Split(string(data), "\n")

	for _, line := range lines {
		switch {
		case strings.HasPrefix(line, "seeds: "):
			inputStr, _ := strings.CutPrefix(line, "seeds: ")
			seeds := strings.Split(inputStr, " ")
			for i, seed := range seeds {
				val, valErr := strconv.Atoi(strings.TrimSpace(seed))
				if valErr != nil {
					log.Fatal(err)
				}
				if i%2 == 0 {
					r := seedRange{
						start: val,
					}
					inputRange = append(inputRange, r)
				} else {
					r := &inputRange[len(inputRange)-1]
					r.end = val
				}
			}
		case line == "" || line[0] == '\r':
		case line[0] >= '0' && line[0] <= '9':
			values := strings.Split(line, " ")

			dstVal, _ := strconv.Atoi(strings.TrimSpace(values[0]))
			srcVal, _ := strconv.Atoi(strings.TrimSpace(values[1]))
			rangeVal, _ := strconv.Atoi(strings.TrimSpace(values[2]))
			transform := transform{
				srcStart: srcVal,
				dstStart: dstVal,
				_range:   rangeVal,
			}

			currentTransform = append(currentTransform, transform)
		default:
			transforms = append(transforms, currentTransform)
			currentTransform = []transform{}
		}
	}
	transforms = append(transforms, currentTransform)
	transforms = transforms[1:]

	for _, transform := range transforms {
		sort.SliceStable(transform, func(i, j int) bool {
			return transform[i].srcStart < transform[j].srcStart
		})
	}

	min := math.MaxInt
	for _, s := range inputRange {

		for i := s.start; i < s.start+s.end; i += 1 {
			// input = append(input, i)
			// for _, v := range input {
			currInput := i
			for j, t := range transforms {
				currInput = transformInput(currInput, t)

				if j == len(transforms)-1 && currInput < min {
					fmt.Println("Result of transformation is :", currInput)
					min = currInput
				}
			}
			// }
			// transformInputOpt(s, input, t)
		}

		// input = input[:0]
	}

	fmt.Println(min)

	// sort.SliceStable(input, func(i, j int) bool {
	// 	return input[i] < input[j]
	// })

	// fmt.Println(input[0])
}

func transformInput(input int, transforms []transform) int {
	for _, transform := range transforms {
		if input >= transform.srcStart && input < transform.srcStart+transform._range {
			return transform.dstStart + (input - transform.srcStart)
		}
	}

	// fmt.Printf("Can't map %d\n", input)
	return input
}

// func transformInputOpt(r seedRange, buf []int, transfoms []transform) {
// 	// f := func(r seed) {

// 	// }()

// 	// index := 0
// 	tStart := 0
// 	for i, transform := range transfoms {
// 		if r.start >= transform.srcStart {
// 			tStart = i
// 			break
// 		}
// 	}

// 	for i := r.start; i < r.end; i += 1 {
// 		buf = append(buf, transformInput(i, transfoms[tStart:]))
// 		// index += 1
// 	}
// }
