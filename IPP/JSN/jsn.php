<?php

#JSN:xcypri01

/**
 * Script to convert json format to XML.
 *
 * @file jsn.php
 * @author Michal Cyprian <xcypri01@stud.fit.vutbr.cz>
 */

$HELP_MSG = <<<EOT
    \nJSN: JSON2XML

    Script to convert JSON format to XML. Each of the item in JSON format 
    (object, array, double name-value) is replaced by one pair of elements.
    Name of double become name of tag, value became a content. Each array in output XML
    is marked by <array> pair of elements and each of the items is marked by <item>
    pair of elements. 
    Transformation of string number values and literals true, false and null depends on
    the options of the script to the attribute value of the element, value in the same
    format as it is in JSON input or the text element. If the value is a floating point number
    it will be rounded down.

    Options:
        --help Show this message and exit.
        --input=filename input file containing JSON data in UTF-8 encoding.
        --output=filename output XML file in UTF-8 encoding.
        -h=subst Each illegal character of xml tag will be replaced by subst (default: _).
        -n XML header will not be generated.
        -r=root-element name of root pair of the element, if option is missing root element
                        will not be added.
        --array-name=array-element set name of pair of element marking array (default: array).
        --item-name=item-element set name of pair of element marking item (default: item).
        -s string values (in double or array) will be transformed to text elements 
           istead of attributes.
        -i number values (in double or array) will be transformed to text elements 
           istead of attributes.
        -l values of literals (true, false, null) will be transformed to elements (<true/>, <false/>
           and <null/> instead of attributes.
        -c activate subtitution of problematic characters using metacharacter & 
           (for example &amp; &lt;, &gt;, etc.).
        -a --array-size attribute size showing length of each array will be added.
        -t --index-items index attribut showing order of items in array will be added.
        --start=n counter of indexes in array will be initialized to n, require --index-items option
                  to be set.
           
EOT;

/**
 * Process command line options, sets nonspecified options to implicit values.
 * If commad line options are invalid prints message to stderr and exit script.
 *
 * @return array of commad line options.
 */
function get_options($argv) {
    global $HELP_MSG;
    $shortopts = "h:nr:silcat";
    $longopts = array("help", "input:", "output:", "array-name:", "item-name:", "array-size", "index-items", "start:");

    $options = getopt($shortopts, $longopts);

    // Comparision of $options and argv to find unexpected commad line arguments
    foreach($argv as $item) {
        if (preg_match("/jsn.php/", $item))
            continue;

        $words = preg_split("/=/", $item);
        $words[0] = preg_replace("/-/", '', $words[0], 2);
        if (!array_key_exists($words[0], $options)) {
            fwrite(STDERR, "Invalid command line options\n");
            exit(1);
        } elseif (count($words) > 1 and (!$options[$words[0]] == $words[1])) {
                fwrite(STDERR, "Invalid command line options\n");
                exit(1);
        }
    }

    if ($options == array("help" => false)) {
        print($HELP_MSG);
        exit(0);
    } elseif (array_key_exists("help", $options)) {
        fwrite(STDERR, "Invalid command line options\n");
        exit(1);
    }

    // Sets missing options to implicit values
    if (!array_key_exists("input", $options))
        $options["input"] = 'php://stdin';
    if (!array_key_exists("output", $options))
        $options["output"] = 'php://stdout';
    if (!array_key_exists("array-name", $options))
       $options["array-name"] = 'array';
    else {
        if (!isValidXmlName($options["array-name"])) {
            fwrite(STDERR, "Invalid value of array-name option\n");
            exit(50);
        }
    }
    if (!array_key_exists("item-name", $options)) 
        $options["item-name"] = 'item';
    else {
        if (!isValidXmlName($options["item-name"])) {
            fwrite(STDERR, "Invalid value of item-name option\n");
            exit(50);
        }
    }
    if (array_key_exists("r", $options))
        if (!isValidXmlName($options["r"])) {
            fwrite(STDERR, "Invalid value of r option\n");
            exit(50);
        }

    if (!array_key_exists("h", $options))
        $options["h"] = '_';

    // Sets alias options
    if (array_key_exists("a", $options))
        $options["array-size"] = false;
    if (array_key_exists("t", $options))
        $options["index-items"] = false;

    if (array_key_exists("start", $options)) {
        if (!array_key_exists("index-items", $options) or !is_numeric($options["start"]) or (int)$options["start"] < 0) {
            fwrite(STDERR, "Invalid command line options\n");
            exit(1);
        } 
    } else
        $options["start"] = 1;

    return $options;
} 

/**
 * Decodes json data from input_file, using JSON extension.
 * Prints message to stderr end exit script if fail to open given file.
 *
 * @param input_file File containing json data
 * @return object containig data from json
 */
function get_json_data($input_file) {
    $json_data = @file_get_contents($input_file);
    if ($json_data == FALSE) {
        fwrite(STDERR, "Failed to open input file $input_file\n");
        exit(2);
    }

    $obj = json_decode($json_data);
    if ($obj == NULL) {
        fwrite(STDERR, "Failed to decode json\n");
        exit(4);
    }

    return $obj;
}

/**
 * Checks if given name is valid name of xml element
 *
 * @param name string to be checked
 * @return True if name is valid xml element, False otherwise
 */
function isValidXmlName($name) {
    try {
        new DOMElement($name);
        if (!strcasecmp(substr($name, 0, 3), "xml"))
            throw new DOMException;
        return True;
    } catch(DOMException $e) {
        return False;
    }
}

/**
 * Class containig various methods to generate XML format from php object.
 * Format of output XML depends on setting stored in options attribute, 
 * includes methods to correct name of xml element if it is invalid.
 * XML creation is based on XMLWriter extension.
 */
class XMLConvertor {
    private static $literal_conversion; 
    private $indent;
    private $opened_elements;
    private $elem_closed_flags;
    private $exit_indent_flags;

    function __construct(XMLWriter $writer, $options) {
        $this->literal_conversion = array(true => "true", false => "false", null => "null");
        $this->indent = 0;
        $this->options = $options;
        $this->opened_elements = array();
        $this->elem_closed_flags = array();
        $this->exit_indent_flags = array();

        $this->xml_writer = $writer;
        $this->xml_writer->openMemory();
        if (!array_key_exists("n", $options))
            $this->xml_writer->startDocument('1.0', 'UTF-8');
        $this->xml_writer->setIndent(4);
        if (array_key_exists("r", $options)) {
            $this->options["r"] = $this->checkXmlName($options["r"]);
                $this->start_element($this->options["r"]);
        }
    }

    /**
     * Controls if argument name is valid XML element name, tries to correct it 
     * if it is not. If the result of correction is invalid element prints 
     * error message to stderr and exits script.
     *
     * @param name potential name of XML element
     * @return original name or name after correction
     */
    public function checkXmlName($name) {
        if (isValidXmlName($name))
            return $name;
        $sub = $this->options["h"];

        # replace prefix xml|XML|Xml if present
        if (!strcasecmp(substr($name, 0, 3), "xml")) {
            $one = 1;
            $name = str_replace(substr($name, 0, 3), $this->options["h"], $name, $one);
        }

        # replace first character if it is not letter or _
        if (!ctype_alpha($name[0]) and ! '_' === ctype_alpha($name[0])) {
            $name = substr($name, 1);
            $name = $this->options["h"] . $name;
        }

        # replace all illegal characters in name
        $name = preg_replace("/[^A-Za-z0-9\-_\.]/", $this->options["h"], $name);


        if (!isValidXmlName($name)) {
            fwrite(STDERR, "Invalid element after substitution $name\n");
            exit(51);
        }
        return $name;
    }

    /**
     * Writes tag to mark start of the element, if option -c is set uses
     * XMLWriter method, otherwise uses own implementation.
     *
     * @param elem name of the element
     */
    public function start_element($elem) {
        if (array_key_exists("c", $this->options))
            $this->xml_writer->startElement($elem);
        else {
            if (!empty($this->elem_closed_flags) and !end($this->elem_closed_flags)) {
                $this->xml_writer->writeRaw(">\n");
                $index = count($this->elem_closed_flags);
                $this->elem_closed_flags[$index -1] = True;
            }
            if (!empty($this->exit_indent_flags) and !end($this->exit_indent_flags)) {
                $index = count($this->exit_indent_flags);
                $this->exit_indent_flags[$index -1] = True;
            }
            $spaces = str_repeat(" ", $this->indent);
            $this->xml_writer->writeRaw("$spaces<$elem");
            $this->indent += 2;
            array_push($this->opened_elements, $elem);
            array_push($this->elem_closed_flags, False);
            array_push($this->exit_indent_flags, False);
        }
    }

    /**
     * Writes tag to mark end of the element, if option -c is set uses
     * XMLWriter method, otherwise uses own implementation.
     */
    public function end_element() {
        if (array_key_exists("c", $this->options))
            $this->xml_writer->endElement();
        else {
            $this->indent -= 2;
            $elem_to_end = array_pop($this->opened_elements);
            if (end($this->elem_closed_flags)) {
                $spaces = str_repeat(" ", $this->indent);
                if (end($this->exit_indent_flags))
                    $this->xml_writer->writeRaw("$spaces</$elem_to_end>\n");
                else
                    $this->xml_writer->writeRaw("</$elem_to_end>\n");
            } else
                $this->xml_writer->writeRaw("/>\n");
            array_pop($this->elem_closed_flags);
            array_pop($this->exit_indent_flags);
        }
    }

    /**
     * Writes attribute, if option -c is set uses XMLWriter method, 
     * otherwise uses writeRaw.
     *
     * @param attr name of attribute
     * @param value of attribute
     */
    public function write_attribute($attr, $value) {
        if (array_key_exists("c", $this->options))
            $this->xml_writer->writeAttribute($attr, $value);
        else
            $this->xml_writer->writeRaw(" $attr=\"$value\"");
    }

    /**
     * Writes text, if option -c is set uses XMLWriter method, 
     * otherwise uses writeRaw.
     *
     * @param value text to be written to current element
     */
    public function write_text($value) {
        if (array_key_exists("c", $this->options))
            $this->xml_writer->text($value);
        else {
            if (!end($this->elem_closed_flags)) {
                $this->xml_writer->writeRaw(">");
                $index = count($this->elem_closed_flags);
                $this->elem_closed_flags[$index -1] = True;
            }
            $this->xml_writer->writeRaw($value);
        }
    }

    /**
     * Choice of operation to be performed depends on type of object
     *
     * @param data object to be converted to XML format
     */
    public function select_operation($data) {
        if (is_object($data))
            $this->object_to_xml($data);
        elseif (is_array($data))
            $this->array_to_xml($data);
        else
            $this->process_content($data);
    }

    /** 
     * Writes given data to XML text content or attributes.
     * Behaviour depends on attribute options
     *
     * @param val data to be written to XML
     */
    public function process_content($val) {

        if (is_float($val))
            $val = intval(floor($val));  # Rounds float down
        if (is_int($val)) {
            if (array_key_exists("i", $this->options)) {
                $this->write_text($val);
            } else 
                $this->write_attribute("value", $val);
        } elseif (is_string($val)) {
            if (array_key_exists("s", $this->options)) {
                $this->write_text($val);
            } else {
                $this->write_attribute("value", $val);
            }
        } elseif (in_array($val, array(true, false, null))) {
            $val = $this->literal_conversion[$val];
            if (array_key_exists("l", $this->options)) {
                $this->start_element($val);
                $this->end_element();
            } else {
                $this->write_attribute("value", $val);
            }
        }
    }
    
    /**
     * Iterates over keys and values of object, creates tags to mark start of 
     * each element and tag to end element after it is processed.
     *
     * @param object to be processed
     */
    public function object_to_xml($object) {
        foreach($object as $key => $value) {
            $key = $this->checkXmlName($key);
            $this->start_element($key);
            $this->select_operation($value);
            $this->end_element();
        }
    }

   /**
    * Iterates over array, creates element start and end tags, incremets index 
    * of item after each iteration
    *
    * @param array to be processed
    */
    public function array_to_xml($array) {
        $this->start_element($this->options["array-name"]);
        if (array_key_exists("array-size", $this->options))
            $this->write_attribute("size", count($array));
        $index = $this->options["start"];
        foreach($array as $item) {
            $this->start_element($this->options["item-name"]);
            if (array_key_exists("index-items", $this->options))
                $this->write_attribute("index", $index);
            $this->select_operation($item, $index);
            $this->end_element();
            $index++;
        }
        $this->end_element();
    }
    /**
     * Method to finish conversion and writes XML to file.
     *
     * @param output_file resulting xml will be written to this file.
     */
    public function flush($output_file) {
        if (array_key_exists("r", $this->options))
            $this->end_element();
        $iRet = @file_put_contents("$output_file", $this->xml_writer->outputMemory());
        if (!$iRet) {
            fwrite(STDERR, "Failed to open output file: $output_file for writting\n");
            exit(3);
        }
        @file_put_contents("$output_file", $this->xml_writer->flush(true), FILE_APPEND);
    }
}

$options = get_options($argv);

$data = get_json_data($options["input"]);

$convertor = new XMLConvertor(new XMLWriter, $options);
$convertor->select_operation($data);
$convertor->flush($options["output"]);
?>
