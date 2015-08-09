<?php
/**
 * Created by PhpStorm.
 * User: rrf011
 * Date: 8/4/2015
 * Time: 9:58 AM
 */

function bullpen_upload_form($form, &$form_state) {
  $form = array();

  /*
   * There was an entity ID (node) passed in the URL so we know they want to add
   * photos to a specific node. We'll grab the info from the db and make the title
   * field hidden and display that title for the user instead. Otherwise, we'll
   * give them an auto complete field and let the user "search" for the node they
   * want to add photos to.
   */
  if (isset($_GET['eid']) && is_numeric($_GET['eid'])) {
    $eid = (int)$_GET['eid'];
    $types = variable_get('bullpen_supported_node_types', array());
    $result = db_select('node', 'n')
      ->fields('n', array('title', 'type'))
      ->condition('n.nid', $eid, '=')
      ->execute();
    if($result->rowCount() > 0) {
      $title = $result->fetchAssoc();

      if (in_array($title['type'], $types)) {
        $title = $title['title'];

        $form['title_id'] = array(
          '#type' => 'value',
          '#value' => $eid,
        );

        $form['title_text'] = array(
          '#prefix' => '<h2 class="bullpen-node-title">',
          '#suffix' => '</h2>',
          '#markup' => t('Add file(s) to <em>@title</em>', array('@title' => $title)),
        );

        /*
         * Since the user came from a specific node, we'll send them back there when
         * their upload is complete.
         */
        //$form['destination']['path'] = 'node/' . $eid;

      } else {
        drupal_set_message(t('You are not allowed to upload files to @name. Default behavior provided below.', array('@name' => $title['title'])), 'warning');
      }
    } else {
      drupal_set_message(t('Invalid entity ID @eid. Default behavior provided below.', array('@eid' => $eid)), 'warning');
    }
  }

  // If there was not an entity ID specified in the URL or there wasn't
  //a matching (allowed) entity, add the title field autocomplete.
  if (!isset($form['title_id'])) {
    $form['title'] = array(
      '#type' => 'textfield',
      '#title' => t('Node'),
      '#description' => t('Begin typing the name of the node that you wish to submit files for. The autocomplete field will display nodes who\'s title is similiar and allow you to select from the list.'),
      '#default_value' => '',
      '#autocomplete_path' => 'bullpen_autocomplete',
      '#required' => TRUE,
    );
  }


  $form['bullpen'] = array(
    '#type' => 'plupload',
    '#title' => t('Upload files'),
    '#description' => t('This multi-upload widget uses Plupload library.'),
    '#upload_validators' => array(
      'file_validate_extensions' => array('jpg jpeg png'),
    ),
    '#plupload_settings' => array(
      'runtimes' => 'html5',
      'chunk_size' => '1mb',
    ),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );

  return $form;
}

function bullpen_upload_form_submit($form, &$form_state) {
  global $user;
  $entity_id = isset($form_state['values']['title_id']) ? $form_state['values']['title_id'] : _bullpen_get_entity_id_from_title($form_state['values']['title']);
  $node = node_load($entity_id);
  $upload_request = array(
    'entity_id' => $entity_id,
    'description' => '',
    'uid' => $user->uid,
    'created' => REQUEST_TIME,
    'changed' => REQUEST_TIME,
  );
  $rid = db_insert('bullpen_request')->fields($upload_request)->execute();

  $upload_path = variable_get('bullpen_path', '');
  if (substr($upload_path, (strlen($upload_path)-1), 1) !== '/') {
    $upload_path .= '/';
  }

  $file_data = array();
  $val_format = '(%d, %d, %d, %d, %d)';


  foreach ($form_state['values']['bullpen'] as $uploaded_file) {
    if ($uploaded_file['status'] == 'done') {
      $source = $uploaded_file['tmppath'];
      $destination = file_stream_wrapper_uri_normalize('private://' . $upload_path . $uploaded_file['name']);
      // Rename it to its original name, and put it in its final home.
      // Note - not using file_move here because if we call file_get_mime
      // (in file_uri_to_object) while it has a .tmp extension, it horks.
      $destination = file_unmanaged_move($source, $destination, FILE_EXISTS_RENAME);
      $file = plupload_file_uri_to_object($destination);
      file_save($file);

      $file_data[] = sprintf($val_format, $rid,$file->fid,0,REQUEST_TIME,REQUEST_TIME);
    }
    else {
      // @todo: move this to element validate or something and clean up t().
      form_set_error('bullpen', "Upload of {$uploaded_file['name']} failed");
    }
  }

  $sql = 'INSERT INTO {bullpen_files} (rid, fid, status, created, changed) VALUES';

  $sql .= implode(',', $file_data);

  db_query($sql);

  drupal_set_message(t('Successfully uploaded @count files to <em>@name</em>', array('@count' => count($file_data), '@name' => $node->title)), 'status');
}

function bullpen_upload_form_validate($form, &$form_state) {
  if (!isset($form_state['values']['title_id'])) {
    if (! _bullpen_get_entity_id_from_title($form_state['values']['title']) ) {
      form_set_error('entity_id', t('Invalid node &mdash; node @title does not exist!', array('@title' => $form_state['values']['title'])));
    }
  }
}

function bullpen_request_list_page($node) {
  global $user;
  $build = array();

  // Get the bullpen requests for this node.
  $result = db_select('bullpen_request', 'r')
    ->fields('r', array('rid'))
    ->condition('r.entity_id', $node->nid, '=')
    ->orderBy('r.created', 'ASC')->execute();
  $rows = array();
  $header = array(
    '',
    t('Submitted by'),
    t('Date'),
    t('# Files'),
    array('data' => t('Operations'))
  );

  $colspan = 1;

  if ($result->rowCount() > 0) {
    foreach (array_keys($result->fetchAllAssoc('rid')) as $rid ) {
      $req = load_bullpen_item($rid);
      $row = array(
        t('Request @rid', array('@rid' => $req->rid)),
        l($req->author->name, 'user/' . $req->author->uid),
        format_date($req->created, 'short'),
        count($req->files),
        l(t('view'), current_path() . '/' . $req->rid),
      );
      if (user_access('administer bullpen requests', $user)) {
        $row[] = l(t('delete'), current_path() . '/' . $req->rid . '/delete');
        if ($colspan == 1) {
          $colspan = 2;
        }
      }
      $rows[] = $row;
    }
  }

  $header[4]['colspan'] = $colspan;

  $build['list_text'] = array(
    '#prefix' => '<div class="bullpen-list-text-wrap">',
    '#suffix' => '</div>',
    '#markup' => '<p>' . t('Below is a listing of file upload requests for this node. Please review each request and approve the individual files if they meet the appropriate criteria.') . '</p>'
  );

  $build['list_table'] = array(
    '#theme' => 'table',
    '#header' => $header,
    '#rows' => $rows,
    '#empty' => t('There are currently no file requests.')
  );

  return $build;
}

function bullpen_item_approval_form($form, $form_state, $bullpen_item) {
  $form = array();

  $form['#bullpen'] = $bullpen_item;
  $form['#node'] = node_load($bullpen_item->entity_id);

  $header = array(
    t('Thumbnail'),
    t('File size')
  );
  $rows = array();

  foreach ($bullpen_item->files as $file) {
    $html = theme('image_style', array('style_name' => 'thumbnail', 'path' => $file['file']->uri));

    $rows[$file['file']->fid] = array(
      $html,
      format_size($file['file']->filesize),
    );
  }

  $form['approval_instructions'] = array(
    '#prefix' => '<div class="approval-instructions-wrap">',
    '#suffix' => '</div>',
    '#markup' => '<p>' . t('To approve the images submitted by @user, check the box next to the approved images and click the submit button.', array('@user' => $bullpen_item->author->name)) . '</p>'
  );

  $form['images_table'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No images found.')
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
    '#access' => count($bullpen_item->files)
  );

  return $form;
}

function bullpen_item_approval_form_submit($form, $form_state) {
  $bullpen = $form['#bullpen'];
  $node = $form['#node'];

  $batch = array(
    'finished' => 'bullpen_batch_finished',
    'title' => t('Assigning files to @title', array('@title' => $node->title)),
    'init_message' => t('Setting up transaction ...'),
    'progress_message' => t('Processed @current out of @total.'),
    'error_message' => t('Bullpen transfer has encountered an error.'),
    'file' => drupal_get_path('module', 'bullpen') . '/bullpen.batch.inc',
  );
  $operations = $files_to_add = $files_to_delete = array();

  foreach ($form_state['values']['images_table'] as $key => $value) {
    $file = file_load($key);
    /*
     * We don't want images that were not selected to be added to the node. These
     * will be considered as "not approved" and therefore will be deleted.
     */
    if ( (int) $form_state['values']['images_table'][$key] > 0 ) {
      $files_to_add[] = $file;
    } else {
      $files_to_delete[$key] = $file;
    }
  }

  if ( count($files_to_add) ) {
    $operations[] = array( 'bullpen_process_approved_file', array($files_to_add, $bullpen, $node) );
  }
  if ( count($files_to_delete) ) {
    $operations[] = array( 'bullpen_process_denied_file', array($files_to_delete, $bullpen) );
  }

  $operations[] = array( 'bullpen_batch_cleanup', array($bullpen) );


  $batch['operations'] = $operations;

  batch_set($batch);
  batch_process('node/' . $node->nid . '/bullpen');
}

function bullpen_item_delete($bullpen) {
  $destination = 'node/' . $bullpen->entity_id . '/bullpen';

  $files = array();
  foreach ($bullpen->files as $key => $value) {
    $files[$key] = $bullpen->files[$key]['file'];
  }

  if (count($files)) {
    bullpen_delete_files($files);
  }
  bullpen_remove_request($bullpen->rid);

  drupal_set_message(t('Bullpen request #@rid (submitted by @author) was deleted.', array('@rid' => $bullpen->rid, '@author' => $bullpen->author->name)));

  drupal_goto($destination);
}

/**
 * Menu callback to get published node titles for files to be attached to. Adapted
 * from the @see user_autocomplete() in the user module.
 * @param string $name
 */
function bullpen_autocomplete($string='') {
  // @todo load allowed types from settings.
  $types = array();
  $matches = array();
  if ($string) {
    $query = db_select('node', 'n')
      ->fields('n', array('title'))
      ->condition('n.title', db_like($string) . '%', 'LIKE')
      ->condition('n.status', 1, '=');

    if ( count($types) ) {
      $query->condition('n.type', $types, 'IN');
    }

    $result = $query->range(0, 10)->execute();

    foreach ($result as $node) {
      $matches[$node->title] = check_plain($node->title);
    }
  }

  drupal_json_output($matches);
}