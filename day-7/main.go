package main

import (
	"fmt"
	"log"
	"os"
	"sort"
	"strconv"
	"strings"
)

type hand struct {
	cards string
	_type handType
	bid   int
	rank  int
}

type card struct {
	val   rune
	count int
}

type handType int

const (
	handInvalid handType = iota
	handFiveOf
	handFourOf
	handFullHouse
	handThreeOf
	handTwoPair
	handOnePair
	handHighCard
)

var cardIndices = map[rune]int{
	'J': 0,
	'2': 1,
	'3': 2,
	'4': 3,
	'5': 4,
	'6': 5,
	'7': 6,
	'8': 7,
	'9': 8,
	'T': 9,
	'Q': 10,
	'K': 11,
	'A': 12,
}

func main() {
	hands := []hand{}

	data, err := os.ReadFile("input.txt")
	if err != nil {
		log.Fatal("Failed to read file")
		return
	}

	lines := strings.Split(string(data), "\n")

	for _, line := range lines {
		if line == "" || line[0] == '\r' || line[0] == '#' {
			continue
		}

		values := strings.Split(line, " ")

		bid, pErr := strconv.Atoi(strings.TrimSpace(values[1]))
		if pErr != nil {
			log.Fatal(pErr)
		}

		h := hand{
			cards: values[0],
			bid:   bid,
		}

		hands = append(hands, h)
	}

	for i := range hands {
		hand := &hands[i]
		hand._type = parseHand(hand)
	}

	sort.SliceStable(hands, func(i, j int) bool {
		h1 := hands[i]
		h2 := hands[j]

		if h1._type == h2._type {
			if strings.Contains(h1.cards, "J") || strings.Contains(h2.cards, "J") {
				fmt.Println(h1, h2)
			}

			for i, c := range h1.cards {
				idx := cardIndices[c]
				otherIdx := cardIndices[rune(h2.cards[i])]

				if idx != otherIdx {
					return idx < otherIdx
				}
			}
		}

		return h1._type > h2._type
	})

	total := 0
	for i := range hands {
		total += hands[i].bid * (i + 1)
		hands[i].rank = i + 1
		fmt.Println(hands[i])
	}

	fmt.Println(total)
}

func parseHand(h *hand) (result handType) {
	t := [13]card{}

	for _, c := range h.cards {
		idx := cardIndices[c]
		t[idx].val = c
		t[idx].count += 1
	}

	jCount := t[cardIndices['J']].count

	sort.SliceStable(t[:], func(i, j int) bool {
		// if t[i].count == t[j].count {
		if t[i].val == 'J' {
			return false
		} else if t[j].val == 'J' {
			return true
		}
		// }

		return t[i].count > t[j].count
	})

	if t[0].val != 'J' {
		t[0].count += jCount
	}

	switch t[0].count {
	case 5:
		result = handFiveOf
	case 4:
		result = handFourOf
	case 3:
		if t[1].count == 2 {
			result = handFullHouse
		} else {
			result = handThreeOf
		}
	case 2:
		if t[1].count == 2 {
			result = handTwoPair
		} else {
			result = handOnePair
		}
	case 1:
		result = handHighCard
	}

	return
}
