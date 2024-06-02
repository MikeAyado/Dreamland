<?php

/**
 * Field handler to present an add to cart form for the product.
 */
class commerce_custom_views_add_to_cart_handler_field_add_to_cart_form extends commerce_cart_handler_field_add_to_cart_form {

  function option_definition() {
    $options = parent::option_definition();

    $options['commerce_custom_views_add_to_cart'] = array(
      'contains' => array(
        'custom_display_path' => array(
          'default' => FALSE,
          'bool' => TRUE,
        ),
        'display_path' => array(
          'default' => '',
          'translatable' => TRUE,
        ),
      )
    );

    return $options;
  }

  /**
   * Provide the add to cart display options.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);

    $form['commerce_custom_views_add_to_cart'] = array(
      '#title' => t('Add to Cart form customizations'),
      '#type' => 'fieldset',
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#weight' => 50,
      '#tree' => TRUE,
    );

    $form['commerce_custom_views_add_to_cart']['custom_display_path'] = array(
      '#type' => 'checkbox',
      '#title' => t('Use custom display path'),
      '#description' => t('Enable to override the display path with custom text or replacement tokens.'),
      '#default_value' => $this->options['commerce_custom_views_add_to_cart']['custom_display_path'],
    );

    $form['commerce_custom_views_add_to_cart']['display_path'] = array(
      '#type' => 'textfield',
      '#title' => t('Custom display path'),
      '#description' => t('The display path to use. You may enter data from this view as per the "Replacement patterns" under "Rewrite results" below.'),
      '#default_value' => $this->options['commerce_custom_views_add_to_cart']['display_path'],
      '#dependency' => array(
        'edit-options-h-add-to-cart-custom-display-path' => array(1),
      ),
    );

    // Hide the default display path setting if we are overriding.
    $form['display_path']['#dependency']['edit-options-commerce-custom-views-add-to-cart-custom-display-path'] = array(0);

    // Enable showing the replacement patterns.
    $form['alter']['help']['#dependency']['edit-options-commerce-custom-views-add-to-cart-custom-display-path'] = array(1);
  }

  function render($values) {
    // Attempt to load the specified product.
    $product = $this->get_value($values);

    if (!empty($product)) {
      // Extract a default quantity for the Add to Cart form line item.
      $default_quantity = $this->options['default_quantity'] <= 0 ? 1 : $this->options['default_quantity'];
      $product_ids = array($product->product_id);

      // Build the line item we'll pass to the Add to Cart form.
      $line_item = commerce_product_line_item_new($product, $default_quantity, 0, array(), $this->options['line_item_type']);
      $line_item->data['context']['product_ids'] = $product_ids;
      $line_item->data['context']['add_to_cart_combine'] = $this->options['combine'];

      // Generate a form ID for this add to cart form.
      $form_id = commerce_cart_add_to_cart_form_id($product_ids);

      $custom = $this->options['commerce_custom_views_add_to_cart'];

      // Add the display path to the line item's context data array if specified.
      if ($custom['custom_display_path'] && $custom['display_path'] !== '') {
        $alter = array(
          'text' => $custom['display_path']
        );
        $tokens = $this->get_render_tokens($alter);
        $display_path = $this->render_altered($alter, $tokens);
        $line_item->data['context']['display_path'] = $display_path;
      }
      elseif ($this->options['display_path']) {
        if ($this->view->display[$this->view->current_display]->display_plugin == 'page') {
          $line_item->data['context']['display_path'] = $this->view->display[$this->view->current_display]->display_options['path'];
        }
        else {
          $line_item->data['context']['display_path'] = current_path();
        }
      }

      // Store the View data in the context data array as well.
      $line_item->data['context']['view'] = array(
        'view_name' => $this->view->name,
        'display_name' => $this->view->current_display,
        'human_name' => $this->view->human_name,
        'page' => $this->view->current_page,
      );

      // Build the Add to Cart form using the prepared values.
      $form = drupal_get_form($form_id, $line_item, $this->options['show_quantity'], array());

      return drupal_render($form);
    }
  }
}
