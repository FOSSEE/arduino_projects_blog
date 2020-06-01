<?php
// $Id$
function list_steps()
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    $intro_query = db_select('arduino_projects_blog_introduction_step');
    $intro_query->fields('arduino_projects_blog_introduction_step');
    $intro_query->condition('proposal_id', $proposal_data->id);
    $intro_query_result  = $intro_query->execute();
    $intro_query_data = $intro_query_result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1 && $proposal_data->approval_status != 4)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been marked for resubmission. Please update your proposal and submit.'), 'status');
                drupal_goto('arduino-projects/my-proposals');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('');
                return;
                break;
        }
      }
      
      
    /*$preference_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $preference_data = db_fetch_object($preference_q);*/
    /*$query = db_select('arduino_projects_blog_preference');
    $query->fields('arduino_projects_blog_preference');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $preference_data = $result->fetchObject();
    if ($preference_data->submited_all_examples_code == 1)
      {
      drupal_set_message(t('You have submited your all codes for this book to review, hence you can not upload more code, for any query please write us.'), 'status');
                drupal_goto('');
                return;       
      }
    if (!$preference_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
      }*/
      if ($proposal_data->submitted_steps == 1)
      {
      drupal_set_message(t('You have submited your all steps for this project to review, hence you can not upload more steps, for any query please write us.'), 'status');
                drupal_goto('');
                return;       
      }
    /************************ end approve book details **************************/
    $return_html = '';
    $return_html .= '<strong>Title of the Book:</strong><br />' . $proposal_data->project_title . '<br /><br />';
    $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->contributor_name . '<br /><br />';
    if($intro_query_data){
    if($intro_query_data->is_submitted == 1){
        $return_html .= '<strong>Introduction</strong><br />';
        $return_html .= '<strong>Description:</strong>' . $intro_query_data->description;
        $return_html .= '<strong>Demo Video:</strong><br />' . convertLinkToEmbed($intro_query_data->demo_video, 450, 350) . '<br />';
    }
    }   
    else{
    $return_html .= l('Update Introduction', 'arduino-projects/code/update-introduction') . '<br />';
    }
    $return_html .= l('Upload Steps', 'arduino-projects/code/upload') . '<br />';
    /* get step list */
    $step_rows = array();
    /*$step_q = db_query("SELECT * FROM {arduino_projects_blog_step} WHERE preference_id = %d ORDER BY number ASC", $preference_data->id);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('proposal_id', $proposal_data->id);
    $query->orderBy('number', 'ASC');
    $step_q = $query->execute();
    while ($step_data = $step_q->fetchObject())
      {
        /* get example list */
        /* $example_q = db_query("SELECT count(*) as example_count FROM {arduino_projects_blog_step_information} WHERE step_id = %d", $step_data->id);
        $example_data = db_fetch_object($example_q);*/
        $query = db_select('arduino_projects_blog_step_information');
        $query->fields('arduino_projects_blog_step_information');
        //$query->addExpression('count(*)', 'example_count');
        $query->condition('step_id', $step_data->id);
        $result = $query->execute();
        $example_data = $result->fetchObject();
        $step_rows[] = array(
            $step_data->number,
            $step_data->name,
            //$step_data->name . ' (' . l('Edit', 'arduino-projects/code/step/edit/' . $step_data->id) . ')',
            //$example_data->example_count,
            l('Edit', 'arduino-projects/code/step/edit/' . $step_data->id) . ' | ' .l('View', 'arduino-projects/code/view-step/' . $step_data->id)
        );
      }
    /* check if there are any steps */
    if (!$step_rows)
      {
        drupal_set_message(t('No uploads found.'), 'status');
        return $return_html;
      }
    $step_header = array(
        'Step No.',
        'Title of the Step',
        //'Uploaded Examples',
        'Actions'
    );
    $return_html .= theme('table', array(
        'header' => $step_header,
        'rows' => $step_rows
    ));
    $step_count = $step_q->rowCount();
    if($step_count >= 5){
     $submited_all_steps = drupal_get_form("all_steps_submitted_check_form",$proposal_data->id);
    $return_html .= drupal_render($submited_all_steps);
    }
    return $return_html;
  }

function update_introduction()
  {
    return drupal_get_form('update_introduction_form');
  }
function update_introduction_form($form, &$form_state)
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1 && $proposal_data->approval_status != 4)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'error');
                drupal_goto('');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
            case 5:
                drupal_set_message(t('You have submitted your all codes.'), 'status');
                drupal_goto('');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('');
                return;
                break;
        }
      }
      $query_intro = db_select('arduino_projects_blog_introduction_step');
      $query_intro->fields('arduino_projects_blog_introduction_step');
      $query_intro->condition('proposal_id', $proposal_data->id);
      $query_intro->condition('approval_status', 0);
      $query_intro_q = $query_intro->execute();
      $query_intro_data = $query_intro_q->fetchObject();
    /*$preference_q = db_query("SELECT * FROM {arduino_projects_blog_preference} WHERE proposal_id = %d AND approval_status = 1 LIMIT 1", $proposal_data->id);
    $proposal_data = db_fetch_object($preference_q);*/
    /*$query = db_select('arduino_projects_blog_preference');
    $query->fields('arduino_projects_blog_preference');
    $query->condition('proposal_id', $proposal_data->id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message(t('Invalid Book Preference status. Please contact site administrator for further information.'), 'error');
        drupal_goto('');
        return;
      }*/
      if($query_intro_data){
        $form['#attributes'] = array(
        'enctype' => "multipart/form-data"
    );
    $form['proposal_id'] =array(
        '#type' => 'hidden',
        '#default_value' => $proposal_data->id
    );
    $form['title'] = array(
        '#type' => 'item',
        '#markup' => t('Introduction'),
        '#title' => t('Title'),
        '#default_value' => t('Introduction')
    );
    $form['description'] = array(
        '#type' => 'text_format',
        '#title' => t('Description'),
        '#required' => TRUE,
        '#format' => $query_intro_data->description_type,
        '#default_value' => $query_intro_data->description
    );
    $form['demo_video'] = array(
        '#type' => 'textarea',
        '#title' => t('Demo video'),
        '#size' => 600,
        '#maxlength' => 600,
        '#required' => TRUE,
        '#default_value' => $query_intro_data->demo_video
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'item',
        '#markup' => l('Cancel', 'arduino-projects/code')
    );
      }
    else{
        $form['#attributes'] = array(
        'enctype' => "multipart/form-data"
    );
    $form['proposal_id'] =array(
        '#type' => 'hidden',
        '#default_value' => $proposal_data->id
    );
    $form['title'] = array(
        '#type' => 'item',
        '#markup' => t('Introduction'),
        '#title' => t('Title'),
        '#default_value' => t('Introduction')
    );
    $form['description'] = array(
        '#type' => 'text_format',
        '#title' => t('Description'),
        '#required' => TRUE,
        '#format' => 'contributor_format'
    );
    $form['demo_video'] = array(
        '#type' => 'textarea',
        '#title' => t('Demo video'),
        '#size' => 600,
        '#maxlength' => 600,
        '#required' => TRUE,
    );
    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
    $form['cancel'] = array(
        '#type' => 'item',
        '#markup' => l('Cancel', 'arduino-projects/code')
    );
}
    return $form;
  }

  function update_introduction_form_submit($form, &$form_state)
  {
    global $user;
    if (!$user->uid)
    {
        drupal_set_message('It is mandatory to login on this website to access the proposal form', 'error');
        return;
    } //!$user->uid
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    $query_intro = db_select('arduino_projects_blog_introduction_step');
    $query_intro->fields('arduino_projects_blog_introduction_step');
    $query_intro->condition('proposal_id', $proposal_data->id);
    $query_intro_q = $query_intro->execute();
    $query_intro_data = $query_intro_q->fetchObject();
    $v = $form_state["values"];
    if($query_intro_data){
        $query = "UPDATE {arduino_projects_blog_introduction_step} SET description = :description, description_type = :description_type, demo_video = :demo_video, creation_date = :creation_date, is_submitted = :is_submitted WHERE id = :id";
        $args = array(
            ":description" => $v['description']['value'],
            ":description_type" => $v['description']['format'],
            ":demo_video" => $v['demo_video'],
            ":creation_date" => time(),
            ":is_submitted" => 1,
            ":id" => $query_intro_data->id
        );
        $result = db_query($query, $args);
    }
    /* inserting the user proposal */
    else{
    $result = "INSERT INTO {arduino_projects_blog_introduction_step} 
    (
    uid, 
    proposal_id,
    title, 
    description,
    description_type,
    demo_video,
    creation_date,
    is_submitted
    ) VALUES
    (
    :uid, 
    :proposal_id, 
    :title, 
    :description,
    :description_type,
    :demo_video,
    :creation_date,
    :is_submitted
    )";
    $args = array(
        ":uid" => $user->uid,
        ":proposal_id" => $v['proposal_id'],
        ":title" => "Introduction",
        ":description" => $v['description']['value'],
        ":description_type" => $v['description']['format'],
        ":demo_video" => $v['demo_video'],
        ":creation_date" => time(),
        ":is_submitted" => 1
        );
    $intro_id = db_query($result, $args, array(
        'return' => Database::RETURN_INSERT_ID
    ));
    }
    drupal_set_message(t('Introduction update successfully.'), 'status');
    drupal_goto('arduino-projects/code');
  }



  function view_steps()
  {
    global $user;
    /************************ start approve book details ************************/
    /*$proposal_q = db_query("SELECT * FROM {arduino_projects_blog_proposal} WHERE uid = %d ORDER BY id DESC LIMIT 1", $user->uid);
    $proposal_data = db_fetch_object($proposal_q);*/
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1 && $proposal_data->approval_status != 4 && $proposal_data->approval_status != 2)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'error');
                drupal_goto('');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('');
                return;
                break;
        }
      }
    /************************ end approve book details **************************/
    /* get step details */
    $step_id = arg(3);
    /*$step_q = db_query("SELECT * FROM {arduino_projects_blog_step} WHERE id = %d AND preference_id = %d LIMIT 1", $step_id, $preference_data->id);*/
    $query = db_select('arduino_projects_blog_step');
    $query->fields('arduino_projects_blog_step');
    $query->condition('id', $step_id);
    $query->condition('proposal_id', $proposal_data->id);
    $query->range(0, 1);
    $step_q = $query->execute();
    if ($step_data = $step_q->fetchObject())
      {
        $return_html = '<strong>Title of the Book:</strong><br />' . $proposal_data->project_title . '<br /><br />';
        $return_html .= '<strong>Contributor Name:</strong><br />' . $proposal_data->contributor_name . '<br /><br />';
        $return_html .= '<strong>Step Number:</strong><br />' . $step_data->number . '<br /><br />';
        $return_html .= '<strong>Title of the step:</strong><br />' . $step_data->name . '<br />';
      }
    else
      {
        drupal_set_message(t('Invalid step.'), 'error');
        drupal_goto('arduino-projects/code');
        return;
      }
    /* get example list */
    $example_rows = array();
    $query = db_select('arduino_projects_blog_step_information');
    $query->fields('arduino_projects_blog_step_information');
    $query->condition('step_id', $step_id);
    $example_q = $query->execute();
    $example_data = $example_q->fetchObject();
      
        /* approval status */
        $approval_status = '';
        switch ($example_data->approval_status)
        {
            case 0:
                $approval_status = 'Pending';
                break;
            case 1:
                $approval_status = 'Approved';
                break;
            case 2:
                $approval_status = 'Rejected';
                break;
        }
        /* example files */
        $example_files = '';
        /*$example_files_q = db_query("SELECT * FROM {arduino_projects_blog_step_files} WHERE example_id = %d ORDER BY filetype", $example_data->id);*/
        $query = db_select('arduino_projects_blog_step_files');
        $query->fields('arduino_projects_blog_step_files');
        $query->condition('step_id', $step_id);
        $query->orderBy('filetype', 'ASC');
        $example_files_q = $query->execute();
        while ($example_files_data = $example_files_q->fetchObject())
          {
            $file_type = '';
            switch ($example_files_data->filetype)
            {
                case 'I':
                    $file_type = 'Images';
                    break;
                case 'G':
                    $file_type = 'Gifs';
                    break;
                default:
            }
            $example_files .= l($example_files_data->filename, 'arduino-projects/download/file/' . $example_files_data->id) . ' (' . $file_type . ')<br />';
          }
        if ($example_data->approval_status == 0)
          {
            $example_rows[] = array(
                'data' => array(
                    $example_data->description,
                   // $video,
                    $approval_status,
                    $example_files,
                    /*l('Edit', 'arduino-projects/code/edit/' . $example_data->id) . ' | ' . l('Delete', 'arduino-projects/code/delete/' . $example_data->id, array(
                        'attributes' => array(
                            'onClick' => 'return confirm("Are you sure you want to delete the example?")'
                        )
                    ))*/
                ),
                'valign' => 'top'
            );
          }
        else
          {
            $example_rows[] = array(
                'data' => array(
                    $example_data->description,
                   // $video,
                    $approval_status,
                    $example_files,
                    l('Download', 'arduino-projects/download/example/' . $example_data->id)
                ),
                'valign' => 'top'
            );
          }
          if(!$example_data->video){
        $video = 'NA';
    }
    else{
        $video = convertLinkToEmbed($example_data->video, 450, 350);
    }
    $example_header = array(
        'Description',
       // 'Video',
        'Status',
        'Files',
        //'Action'
    );
    $return_html .= '<strong>Description:</strong><br />' . $example_data->description . '<br />';
    $return_html .= '<strong>Video:</strong><br />' . $video . '<br />';
    $return_html .= theme('table', array(
        'header' => $example_header,
        'rows' => $example_rows
    ));
   $return_html .= '<br />' . l('Back to step List', 'arduino-projects/code');
    return $return_html;
  }


  function all_steps_submitted_check_form($form,&$form_state,$proposal_id){
    //$form = array();
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('id', $proposal_id);
    $query->condition('approval_status', 1);
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    $form['#attributes'] = array(
        'enctype' => "multipart/form-data"
    );
    $form['all_steps_submitted'] = array(
            '#type' => 'checkbox',
            '#title' => t('I have submitted steps for the project'),
            '#description' => 'Once you have submited this option you are not able to upload more steps.',
            '#required' => TRUE,
    );
    //var_dump($preference_data->approved_codable_example_files);die;
        $form['hidden_preference_id'] = array(
        '#type' => 'hidden',
        '#value' => $proposal_id
    );
        $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t('Submit')
    );
        return $form;
}

function all_steps_submitted_check_form_submit($form,&$form_state){
    global $user;
    $query = db_select('arduino_projects_blog_proposal');
    $query->fields('arduino_projects_blog_proposal');
    $query->condition('uid', $user->uid);
    $query->orderBy('id', 'DESC');
    $query->range(0, 1);
    $result = $query->execute();
    $proposal_data = $result->fetchObject();
    if (!$proposal_data)
      {
        drupal_set_message("Please submit a " . l('proposal', 'arduino-projects/proposal') . ".", 'error');
        drupal_goto('');
      }
    if ($proposal_data->approval_status != 1 && $proposal_data->approval_status != 4 && $proposal_data->approval_status != 2)
      {
        switch ($proposal_data->approval_status)
        {
            case 0:
                drupal_set_message(t('We have already received your proposal. We will get back to you soon.'), 'status');
                drupal_goto('');
                return;
                break;
            case 2:
                drupal_set_message(t('Your proposal has been dis-approved. Please create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'error');
                drupal_goto('');
                return;
                break;
            case 3:
                drupal_set_message(t('Congratulations! You have completed your last book proposal. You have to create another proposal ' . l('here', 'arduino-projects/proposal') . '.'), 'status');
                drupal_goto('');
                return;
                break;
            case 5:
                drupal_set_message(t('You have submmited your all codes'), 'status');
                drupal_goto('');
                return;
                break;
            default:
                drupal_set_message(t('Invalid proposal state. Please contact site administrator for further information.'), 'error');
                drupal_goto('');
                return;
                break;
        }
      }
      db_query('UPDATE arduino_projects_blog_proposal SET submitted_steps = 1 WHERE id = :proposal_id',array(':proposal_id' => $proposal_data->id));

}