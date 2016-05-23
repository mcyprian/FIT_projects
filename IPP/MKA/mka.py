#!/usr/bin/python3
# -*- coding: utf-8 -*-
#MKA:xcypri01

import sys
import re
import itertools
import argparse
import logging

logger = logging.getLogger("MKA")
logger.setLevel("NOTSET")
logger.addHandler(logging.StreamHandler(sys.stderr))


def get_arguments():
    """Gets command line arguments using argparse, check conflicts in
    specified arguments.
    Return: dictionary of specified command lines arguments and its values.
    """
    parser = CustomizedArgumentParser(description='''Script to process and
    minimize finite automata. Script processes text definition of automata,
    validating that automata is well specified and generatig equivalent
    minimized automata.'''
                                      )
    parser.add_argument("--input",
                        required=False,
                        help="Input file containing automata definition.",
                        metavar='filename',
                        action=DuplicitCheck
                        )
    parser.add_argument("--output",
                        required=False,
                        help="Output file, processed automata definition will be written to.",
                        metavar='filename',
                        action=DuplicitCheck,
                        )
    parser.add_argument("-f", "--find-non-finishing",
                        required=False,
                        action=DuplicitCheckStoreTrue,
                        help=("Finds not accept state of input automata "
                              "definition, prints it to output")
                        )
    parser.add_argument("-m", "--minimize",
                        required=False,
                        action=DuplicitCheckStoreTrue,
                        help="Minimizes input automata"
                        )
    parser.add_argument("-i", "--case-insensitive",
                        required=False,
                        action=DuplicitCheckStoreTrue,
                        help=("Names of states and symbols will be case "
                              "insensitive, all lowercased in output.")
                        )
    # Store argumet parsing results as an dictionary
    args = parser.parse_args().__dict__
    logger.info(args)
    if args['find_non_finishing'] and args['minimize']:
        parser.error("invalid combination of arguments -f, "
                     "--find-non-finishing and -m, --minimize.")
    return args


def get_input_file(input_file):
    """Reads and returns content of input file, uses stdin if input_file
    is None.
    """
    try:
        if input_file:
            with open(input_file, 'r', encoding='utf-8') as fi:
                file_content = fi.read()
        else:
            file_content = sys.stdin.read()
    except FileNotFoundError:
        sys.stderr.write("ERROR: Failed to open input file {}.\n".format(input_file))
        sys.exit(2)
    return file_content


def write_output(output_file, output):
    """Writes output string ro output_file, uses stdout if output_file is
    None.
    """
    try:
        if output_file:
            with open(output_file, 'w', encoding='utf-8') as fo:
                fo.write(output)
        else:
            sys.stdout.write(output)
    except (FileNotFoundError, PermissionError):
        sys.stderr.write("ERROR: Failed to open output file {}.\n".format(output_file))
        sys.exit(3)


def normalize_input(aut_definition):
    """Removes comments and whitespaces from input automata
    definition.
    """
    masking_dict = {
        "' '": '&_space',
        "'\t'": '&_tab',
        "'\n'": '&_newline'
    }
    aut_definition = aut_definition.replace("'#'", "'&_octothorpe'")
    comment_regex = re.compile(r'#[^\n]*\n')
    end_comment_regex = re.compile(r'#[^$]*$')
    # remove comments
    aut_definition = comment_regex.sub("", aut_definition)
    aut_definition = end_comment_regex.sub("", aut_definition)
    aut_definition = aut_definition.replace("'&_octothorpe'", "'#'",)
    # mask problemtic characters in automata definition
    for key, value in masking_dict.items():
        aut_definition = aut_definition.replace(key, value)

    # remove all whitespaces
    aut_definition = "".join(aut_definition.split())

    # unmask problematic characters in automata definition
    for key, value in masking_dict.items():
        aut_definition = aut_definition.replace(value, key)

    return aut_definition


def check_id(name, id_regex_list):
    """Checks if name matches given id_regex."""
    if not name:
        return
    for regex in id_regex_list:
        if not regex.match(name):
            logger.info("Bad identifier: {}".format(name))
            raise InputSyntaxError


def automata_from_string(normalized, lowercase=False):
    """Parses normalized string automata definition,
    preforms first checks of syntax and semantics,
    Returns initialized Automata instance.
    """
    normalized = normalized[1:-1]
    id_regex = re.compile('[a-zA-Z]([_a-zA-Z0-9][a-zA-Z0-9])*')
    underscore_end = re.compile('.*[^_]$')
    try:
        states = set(normalized.split('},')[0][1:].split(','))
        raw_symbols = normalized.split('},')[1][1:]
        logger.info("raw_symbols: {}".format(raw_symbols))
        if "''''" not in raw_symbols and "''" in raw_symbols:
            raise InputSyntaxError
        symbols = {','} if raw_symbols == "','" or "',','" in raw_symbols else set()
        symbols |= {symbol[1:-1] for symbol in raw_symbols.split(',') if symbol != "'"}
        # mask comma in rules
        raw_rules = set(normalized.split('},')[2][1:].replace("','", "'&_comma'")
                                                     .split(','))
        logger.info("raw_rules: {}".format(raw_rules))
        rules = set()
        for rule in raw_rules:
            rule = rule.split("'")
            if rule[1] == "&_comma":
                rule[1] = ','
            elif len(rule) == 3 and rule[1] == '':  # epsilon transition
                raise NotWellSpecifiedError
            if len(rule) == 5 and rule[1] == '':  # '''' in symbols
                rule[2] = rule[-1]
                rule[1] = "''"
                del rule[3:]
            rule[2] = rule[2][2:]
            rules.add(tuple(rule))
        start_state, accept_states = normalized.split('},')[3].split(',{')
        accept_states = set(accept_states[:-1].split(','))
        logger.info("states: {}\nsymbols: {}\nrules: {}\nstart_state:"
                    "{}\naccept_states: {}\n".format(states, symbols, rules,
                                                     start_state, accept_states))
        if lowercase:
            states = {state.lower() for state in states}
            symbols = {symbol.lower() for symbol in symbols}
            rules = {(p.lower(), a.lower(), q.lower()) for p, a, q in rules}
            start_state = start_state.lower()
            accept_states = {state.lower() for state in accept_states}

        for state in states | {r[0] for r in rules} | {r[-1] for r in rules} | {start_state} | accept_states:
            check_id(state, [id_regex, underscore_end])
    except (IndexError, ValueError):
        raise InputSyntaxError
    return Automata(states, symbols, rules, start_state,
                    accept_states, well_specified_check=True)


class CustomizedArgumentParser(argparse.ArgumentParser):
    """ArgumentParser customized to return excepted exit codes."""

    def error(self, message):
        """Overridden to return exit code 1 on invalid arguments format"""
        self.print_usage(sys.stderr)
        self.exit(1, "{}: error: {}\n".format(self.prog, message))


class DuplicitCheck(argparse.Action):
    """Raises exception if option is specified more than one."""

    def __call__(self, parser, namespace, values, option_string=None):
        if getattr(namespace, self.dest):
            raise argparse.ArgumentError(None, "{} option specified multiple times.".format(
                option_string))
        setattr(namespace, self.dest, values)


class DuplicitCheckStoreTrue(DuplicitCheck):
    """Duplicit check including additional store_true action behaviour."""

    def __init__(self,
                 option_strings,
                 dest,
                 default=False,
                 required=False,
                 help=None):
        super(DuplicitCheckStoreTrue, self).__init__(
            option_strings=option_strings,
            dest=dest,
            nargs=0,
            const=True,
            default=default,
            required=required,
            help=help)

    def __call__(self, parser, namespace, values, option_string=None):
        super(DuplicitCheckStoreTrue, self).__call__(parser,
                                                     namespace,
                                                     values,
                                                     option_string)
        setattr(namespace, self.dest, self.const)


class SuccessfulSplitException(Exception):
    """Indicates successful split of state subsets"""

    pass


class MKAScriptError(Exception):
    """Base class for script exceptions"""

    exit_code_dict = {
        'MKAScriptError': (1, "Unknow error occured\n"),
        'InputSyntaxError': (60, "Syntax error in automata definition.\n"),
        'InputSemanticError': (61, "Semantic error in automata definition.\n"),
        'NotWellSpecifiedError': (62, "Input automata is not well specified.\n")
    }

    def __init__(self, message=None, exit_code=None):

        if exit_code is None and message is None:
            exit_code, message = self.exit_code_dict[type(self).__name__]

        super(MKAScriptError, self).__init__(message)
        self.exit_code = exit_code


class InputSyntaxError(MKAScriptError):
    pass


class InputSemanticError(MKAScriptError):
    pass


class NotWellSpecifiedError(MKAScriptError):
    pass


class Automata(object):
    """Class representing finite automata 5-tuple"""

    def __init__(self, states=None, symbols=None, rules=None, start_state='',
                 accept_states=None, well_specified_check=False):
        logger.info("Creating automata:\nstates: {}\nsymbols: {}\nrules: {}\n"
                    "start_state: {}\naccept_states: {}\n".format(
                        states, symbols, rules, start_state, accept_states))

        if not accept_states or not accept_states - {''}:
            raise NotWellSpecifiedError

        if not symbols or start_state not in states or not accept_states.issubset(states):
            raise InputSemanticError

        for from_state, symbol, to_state in rules:
            if not {from_state, to_state}.issubset(states) or symbol not in symbols:
                raise InputSemanticError

        self.states = states
        self.symbols = symbols
        self.rules = rules
        self.start_state = start_state
        self.accept_states = accept_states
        # List of sets is initialized with two sets, accept_states and the others
        self.sets_of_states = [accept_states, states - accept_states]
        logger.info("Initial sets_of_states: {}".format(self.sets_of_states))
        if well_specified_check:
            if not self.is_well_specified:
                raise NotWellSpecifiedError

    @property
    def rules_left_part(self):
        """Returns list of left parts of rules: 
        (p, a) from each rule (p, a, q).
        """
        if not hasattr(self, '_rules_left_part'):
            self._rules_left_part = [rule[:-1] for rule in self.rules]
        return self._rules_left_part

    @property
    def is_deterministic(self):
        """Return True if instance contains definition of deterministic
        automata.
        """
        logger.info("is_deterministic:")
        logger.info(len(self.rules_left_part) == len(set(self.rules_left_part)))
        return len(self.rules_left_part) == len(set(self.rules_left_part))

    @property
    def is_complete(self):
        """Return True if instance contains definition of complete automata."""
        logger.info("is_complete")
        for p in self.states:
            for a in self.symbols:
                if (p, a) not in self.rules_left_part:
                    logger.info(False)
                    return False
        logger.info(True)
        return True

    @property
    def has_inaccessible_states(self):
        """Checks if automata has any inaccessible states,
        Return True if one or more states are inaccessible, otherwise False.
        """
        logger.info("has_inaccessible_states")
        for state in self.states:
            if not self.find_path(self.start_state, state):
                logger.info(True)
                return True
        logger.info(False)
        return False

    @property
    def nonterminating_states(self):
        """List of nonterminating states of automata"""
        logger.info("has_nonterminating_states")
        if not hasattr(self, '_nonteminating_state'):
            self._nonterminating_states = []
            for state in self.states:
                nonterminating = [state]
                for accept_state in self.accept_states:
                    if self.find_path(state, accept_state):
                        nonterminating.pop()
                        break
                self._nonterminating_states += nonterminating
        logger.info(self._nonterminating_states)
        return self._nonterminating_states

    @property
    def is_well_specified(self):
        """Return True if instance contains definition of well specified
        automata.
        """
        return (self.is_deterministic and self.is_complete and not self.has_inaccessible_states
                and len(self.nonterminating_states) <= 1)

    @property
    def graph_from(self):
        """Return automata states in form of undirected graph."""
        if not hasattr(self, '_graph_form'):
            self._graph_form = {}
            for state in self.states:
                self._graph_form[state] = [p[2] for p in self.rules if p[0] == state]
        return self._graph_form

    def find_path(self, start, end, path=None):
        """Finds path from start state to end state,
        Return path from starting to ending state or None if path does not
        exists.
        """
        path = path + [start] if path else [start]
        if start == end:
            return path
        if not start in self.graph_from:
            return None
        for node in self.graph_from[start]:
            if node not in path:
                newpath = self.find_path(node, end, path)
                if newpath:
                    return newpath
        return None

    @staticmethod
    def set_repr(input_set):
        """String represenatation of set attributes."""
        return ", ".join(sorted(list(input_set)))

    @staticmethod
    def one_rule_repr(rule):
        """String represenatation of rule."""
        return "{0[0]} '{0[1]}' -> {0[2]}".format(rule)

    @property
    def symbols_repr(self):
        """String represenatation of symbol."""
        return ", ".join(["'{}'".format(s) for s in sorted(list(self.symbols))])

    @property
    def rules_repr(self):
        """String represenatation of all the rules, using one_rule_repr."""
        string_rules = sorted([self.one_rule_repr(rule) for rule in self.rules])
        result_str = ""
        for rule_repr in string_rules:
            result_str += "{},\n".format(rule_repr)
        return result_str[:-2]

    def __str__(self):
        return "(\n{{{}}},\n{{{}}},\n{{\n{}\n}},\n{},\n{{{}}}\n)".format(
            self.set_repr(self.states), self.symbols_repr, self.rules_repr,
            self.start_state, self.set_repr(self.accept_states))

    def get_subsets_states(self, d_rules):
        """Tries to create sets X1, X2 for each of givend rules
        Returns sets on success False on failure.
        """
        subsets = []
        for set_of_states in self.sets_of_states:
            logger.debug("    set_of_states {}".format(set_of_states))
            Q1 = {d_rules[0][2]}
            Q1_set = [s for s in self.sets_of_states if max(Q1) in s][0]
            X1 = {d_rules[0][0]}
            X2 = set()
            for rule in d_rules:
                logger.debug("    rule {}".format(rule))
                if rule[2] in Q1_set:
                    X1.add(rule[0])
                    logger.debug("Q1: {} X1: {}".format(Q1, X1))
                else:
                    # q not in Q1_set
                    X2.add(rule[0])
                    logger.debug("X2: {}".format(X2))
            if X1 and X2:
                logger.debug("Seccess X1 {} X2 {}".format(X1, X2))
                return (X1, X2)
        logger.debug("Unsuccess")
        return (False, False)

    def update_set_of_states(self, new_sets):
        """Creates new subsets of states after successful split."""
        logger.debug("new sets: {} set_of_states: {}".format(new_sets, self.sets_of_states))
        for index, set_of_states in enumerate(self.sets_of_states):
            for new in new_sets:
                if new.issubset(set_of_states):
                    self.sets_of_states[index] = self.sets_of_states[index] - new
                    if not self.sets_of_states[index]:
                        # remove empty set
                        del self.sets_of_states[index]
                break
        self.sets_of_states += list(new_sets)
        # Unique sets of states
        self.sets_of_states = [set(k) for k in {tuple(s) for s in self.sets_of_states}]
        logger.debug("Updated set of states {}".format(self.sets_of_states))

    @staticmethod
    def to_state(to_convert):
        return '_'.join(sorted(list(to_convert)))

    def create_minimal(self):
        """Creates minimized automata from sets of states content after
        minimize call."""
        states = {self.to_state(s) for s in self.sets_of_states}
        rules = set()
        for rule in self.rules:
            for X, Y in itertools.product(self.sets_of_states, repeat=2):
                if rule[0] in X and rule[2] in Y:
                    rules.add((self.to_state(X), rule[1], self.to_state(Y)))

        accept_states = set()
        for X in self.sets_of_states:
            if self.start_state in X:
                start_state = self.to_state(X)
            if X & self.accept_states:
                accept_states.add(self.to_state(X))

        return Automata(states, self.symbols, rules, start_state,
                        accept_states)

    def minimize(self):
        """Performs minimization alghoritm."""
        while True:
            try:
                for set_of_states in self.sets_of_states:
                    logger.debug("subset: {}".format(set_of_states))
                    for d in self.symbols:
                        results = []
                        logger.debug('    D: {}'.format(d))
                        for rule in self.rules:
                            if rule[1] == d and rule[0] in set_of_states:
                                results.append(rule)
                        if results:
                            new_sets = self.get_subsets_states(results)
                            if new_sets[0]:
                                raise SuccessfulSplitException
            except SuccessfulSplitException:
                # Successful split of state subsets iteration will be restarted
                self.update_set_of_states(new_sets)
            else:
                # Can't split anymore, end of loop
                break
        logger.info("set_of_states: {}".format(self.sets_of_states))


if __name__ == '__main__':
    args = get_arguments()
    file_content = get_input_file(args['input'])
    try:
        normalized = normalize_input(file_content)
        input_aut = automata_from_string(normalized, args['case_insensitive'])
        if args['find_non_finishing']:
            output = input_aut.nonterminating_states[0] if input_aut.nonterminating_states else '0'
        elif args['minimize']:
            input_aut.minimize()
            minimized_aut = input_aut.create_minimal()
            output = minimized_aut.__str__()
        else:
            output = input_aut.__str__()
        write_output(args['output'], output)
    except MKAScriptError as e:
        sys.stderr.write("ERROR: {}".format(e))
        sys.exit(e.exit_code)
