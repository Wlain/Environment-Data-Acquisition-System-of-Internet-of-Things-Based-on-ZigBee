package com.springboot.entities;

import javax.persistence.*;
import java.util.Date;

/**
 * Created by 10497 on 2017/6/12.
 */
@Entity
@Table(name = "Gas")
public class Gas {
    @Id
    @GeneratedValue(strategy = GenerationType.IDENTITY)
    private int id;
    private int gas;
    private Date date;

    public void setId(int id) {
        this.id = id;
    }

    public int getId() {
        return id;
    }

    public void setGas(int gas) {
        this.gas = gas;
    }

    public int getGas() {
        return gas;
    }

    public void setDate(Date date) {
        this.date = date;
    }

    public Date getDate() {
        return date;
    }
}
