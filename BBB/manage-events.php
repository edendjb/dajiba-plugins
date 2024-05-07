<?php

$action = "none";

if (isset($_GET['a'])) {
    $action = $_GET['a'];
}


if ($action === "none") {
    // default path, get all events
    $events_file = '/path/to/events.json';
    $fp = fopen($events_file, 'rb');
    header('Content-Type: application/json');
    header('Content-Length: '.filesize($events_file));

    fpassthru($fp);
    fclose($fp);
    exit;
} else if ($action === "s") {
    if (!isset($_GET['e'])) {
        die('No event specified.');
    }
    // check event name
    $event_name = $_GET['e'];
    settype($event_name, 'string');
    if (strlen($event_name) > 32) {
        die('Event name too long.');
    }

    if (!preg_match('/^[-A-Za-z0-9_]+$/', $event_name)) {
        die('Event name invalid.');
    }


    if ($event_name == 'get-all') {
     $events_file = '/path/to/events.json';
     $event_data = file_get_contents($events_file);

     $json_data = json_decode($event_data, true);

     $msg = 'TOD Report:'."\n";

    
     foreach ($json_data as $key => $value) {
        if (in_array($key, array('fafnir','turtle','behe','ka'))) {
            $last_tod = $value['tod'];
            $start_time = $value['start'];
            $interval = $value['interval'];

            $window = 1;

            $curtime = time();
            for ($window = 1; $window <= $value['windows']; ++$window) {
                $timestamp = $last_tod + $start_time + (($window - 1) * $interval);

                if ($timestamp - $curtime > 0) {
                    $dif = $timestamp - $curtime;
                    $hours = 0;
                    $minutes = 0;
                    $seconds = $dif;

                    if ($seconds > 60) {
                        $minutes = floor($seconds / 60);
                        $seconds = $seconds % 60;
                    }
                    if ($minutes > 60) {
                        $hours = floor($minutes / 60);
                        $minutes = $minutes % 60;
                    }

                    $msg .= $key .': ';

                    if ($hours > 0) {
                        $msg .= $hours .'h ';
                    }
                    if ($minutes > 0) {
                        $msg .= $minutes .'m ';
                    }
                    if ($seconds > 0) {
                        $msg .= $seconds .'s ';
                    }

                    $msg .= ' W'.$window.'  (day '.$value['day'].')';

                    $msg .= "\n";

                    break;
                }
            }
        }
     }

    print($msg);

    die();
    }

    if (!isset($_GET['t'])) {
        die('No time specified.');
    }

    $event_time = $_GET['t'];
    if (strlen($event_time) > 32) {
        die('Event time too long.');
    }

    $cmd = 'python3.8 /path/to/set-tod.py ';
    $cmd .= '-e "'.$event_name.'" ';

    if ($event_time != 'same') {
        try {
            // try with forced alertaru format first...
            if (str_contains($event_time, 'ET')) {
                $event_time = str_replace('ET', 'EDT', $event_time);
            }
            if (str_contains($event_time, 'CT')) {
                $event_time = str_replace('CT', 'CDT', $event_time);
            }
            if (str_contains($event_time, 'PT')) {
                $event_time = str_replace('PT', 'PDT', $event_time);
            }
            $time_conversion = DateTime::createFromFormat('m-d-Y H:i:s', $event_time, new DateTimeZone('America/New_York'));
            if (!$time_conversion) {
                // if it fails, they may have done another format
                $time_conversion = new DateTime($event_time, new DateTimeZone('America/New_York'));
                $time_conversion->setTimezone( new DateTimeZone('America/New_York'));
            }
        } catch (Exception $e) {
            die('Could not parse time.  Use "MM/DD/YYYY HH:MM:SS TZ"');
        }

        $event_time = $time_conversion->format('m/d/Y H:i:s T');
    }

    $cmd .= '-t "'.$event_time.'" ';
    
    // day (optional)
    if (isset($_GET['d'])) {
        $d = $_GET['d'];
        if ($d === '+1') {
            $cmd .= '-d +1 ';
        } else {
            settype($d, 'int');
            if ($d < 10 && $d >=0 ){
                $cmd .= '-d '.$d.' ';
            }
        }
    }

    // claimer name
    if (isset($_GET['c'])) {
        $c = $_GET['c'];
        settype($c, 'string');
        if (strlen($c) <= 16 && strlen($c) >=3 && preg_match('/^[A-Z][a-z]+$/', $c)) {
            $cmd .= '-c '.$c.' ';
        }
    }

    // actual target name
    if (isset($_GET['n'])) {
        $n = $_GET['n'];
        settype($n, 'string');
        if (strlen($n) <= 64 && 
            strlen($n) >=3 && 
            preg_match('/^[A-Z]([A-Za-z0-9_-]|\s)+$/', $n)
           ) {
            $cmd .= '-n '.$n.' ';
        }       
    }
    
    // notes?
    if (isset($_GET['x'])) {
        $note = $_GET['x'];
        settype($note, 'string');
        if (strlen($note) <= 50) {
            $cmd .= '-x "'.$note.'" ';
        }
    }

    if (isset($_GET['start'])) {
        $start_time = $_GET['start'];
        settype($start_time, 'int');
        if ($start_time >= 0) {
            $cmd .= '--start '.$start_time.' ';
        }
    }

    if (isset($_GET['interval'])) {
        $interval = $_GET['interval'];
        settype($interval, 'int');
        if ($interval > 0) {
            $cmd .= '--interval '.$interval.' ';
        }
    }

    if (isset($_GET['alerts'])) {
        $alerts = $_GET['alerts'];
        $check_alerts = explode(',', $alerts);
        $all_ints = true;
        for ($i=0; $i<count($check_alerts); $i++) {
            settype($check_alerts[$i], 'int');
            if ($check_alerts[$i] <=0) {
                $all_ints = false;
                break;
            }
        }
        if ($all_ints) {
            $cmd .= '--alerts "'.$alerts.'" ';
        }
    }

    if (isset($_GET['windows'])) {
        $windows = $_GET['windows'];
        settype($windows, 'int');
        if ($windows > 0) {
            $cmd .= '--windows '.$windows.' ';
        }
    }

    exec($cmd, $str);

    echo implode("\n", $str);
    exit;
}


?>
